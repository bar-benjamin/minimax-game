#include "minimax/domain/board.hpp"
#include <stdexcept>
#include <random>

namespace minimax {
namespace domain {

Board::Board(unsigned int seed, std::size_t grid_size)
: seed_(seed)
, grid_size_(grid_size)
{
    if (seed_ == 0) {
        std::random_device rd;
        seed_ = rd();
    }
    rng_.seed(seed_); // initialize seed for board random number generator
    
    grid_.resize(grid_size);
    for (std::size_t i = 0; i < grid_size; ++i) {
        grid_[i].reserve(grid_size);
        for (std::size_t j = 0; j < grid_size; ++j) {
            grid_[i].emplace_back(0);
        }
    }

    generate();
}

const Cell& Board::get_cell(Position pos) const
{
    if (!is_valid_position(pos)) {
        throw std::out_of_range("Error: Invalid position");
    }

    return grid_[pos.row][pos.col];
}

Cell& Board::get_cell(Position pos)
{
    if (!is_valid_position(pos)) {
        throw std::out_of_range("Error: Invalid position");
    }

    return grid_[pos.row][pos.col];
}

bool Board::is_valid_position(Position pos) const
{
    return pos.row < grid_size_ && pos.col < grid_size_;
}

void Board::generate()
{
    generate_blocked_cells();
    generate_special_cells();
    generate_number_cells();
}

void Board::generate_blocked_cells()
{
    std::uniform_int_distribution<std::size_t> pos_dist(0, grid_size_ - 1);
    std::uniform_int_distribution<std::size_t> count_dist(2, MAX_BLOCKED_CELLS);
    
    std::size_t num_blocked = count_dist(rng_);
    for (std::size_t i = 0; i < num_blocked; ++i) {
        Position pos{pos_dist(rng_), pos_dist(rng_)};
        grid_[pos.row][pos.col] = Cell(CellType::BLOCKED);
    }
}

void Board::generate_special_cells()
{
    std::vector<Position> empty_cells;
    for (std::size_t i = 0; i < grid_size_; ++i) {
        for (std::size_t j = 0; j < grid_size_; ++j) {
            if (grid_[i][j].get_type() == CellType::NUMBER && grid_[i][j].get_value() == 0) {
                empty_cells.push_back({i, j});
            }
        }
    }
    
    std::shuffle(empty_cells.begin(), empty_cells.end(), rng_);
    
    std::size_t i;
    for (i = 0; i < BOMBS_NUM && i < empty_cells.size(); ++i) {
        Position pos = empty_cells[i];
        grid_[pos.row][pos.col] = Cell(CellType::BOMB);
    }
    
    for (; i < SURPRISES_NUM && i < empty_cells.size(); ++i) {
        Position pos = empty_cells[i];
        SurpriseBonus bonus = (i % 2 == 0) ? SurpriseBonus::BONUS_20 : SurpriseBonus::BONUS_50;
        grid_[pos.row][pos.col] = Cell(CellType::SURPRISE, bonus);
    }
}

void Board::generate_number_cells()
{
    std::uniform_int_distribution<int> value_dist(MIN_CELL_VALUE, MAX_CELL_VALUE);
    
    for (std::size_t i = 0; i < grid_size_; ++i) {
        for (std::size_t j = 0; j < grid_size_; ++j) {
            if (grid_[i][j].get_type() == CellType::NUMBER && grid_[i][j].get_value() == 0) {
                grid_[i][j] = Cell(value_dist(rng_));
            }
        }
    }
}

std::vector<uint8_t> Board::serialize() const
{
    std::vector<uint8_t> data;
    data.reserve(grid_size_ * grid_size_ * 3); // 3 = type, value, bonus
    
    for (std::size_t i = 0; i < grid_size_; ++i) {
        for (std::size_t j = 0; j < grid_size_; ++j) {
            const Cell& cell = grid_[i][j];
            data.push_back(static_cast<uint8_t>(cell.get_type()));
            data.push_back(static_cast<uint8_t>(cell.get_value()));
            data.push_back(static_cast<uint8_t>(cell.get_bonus()));
        }
    }
    
    return data;
}

void Board::deserialize(std::vector<uint8_t> const& data)
{
    if (data.size() != grid_size_ * grid_size_ * 3) {
        throw std::invalid_argument("Error: Invalid serialized board size");
    }
    
    std::size_t idx = 0;
    for (std::size_t i = 0; i < grid_size_; ++i) {
        for (std::size_t j = 0; j < grid_size_; ++j) {
            CellType type = static_cast<CellType>(data[idx++]);
            int value = static_cast<int8_t>(data[idx++]);
            SurpriseBonus bonus = static_cast<SurpriseBonus>(data[idx++]);
            
            if (type == CellType::NUMBER) {
                grid_[i][j] = Cell(value);
            } else {
                grid_[i][j] = Cell(type, bonus);
            }
        }
    }
}

}
}
