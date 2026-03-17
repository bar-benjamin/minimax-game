#ifndef MINIMAX_DOMAIN_BOARD_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_BOARD_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include "cell.hpp"
#include <vector>
#include <random>

namespace minimax {
namespace domain {

class Board {
public:
    explicit Board(unsigned int seed=DEFAULT_SEED, std::size_t grid_size=GRID_SIZE);

    ~Board() = default;
    Board(const Board& other) = default;
    Board& operator=(const Board& other) = default;
    
    const Cell& get_cell(Position pos) const;
    Cell& get_cell(Position pos);

    bool is_valid_position(Position pos) const;
    
    /**
     * @brief Generate board with random cells
     */
    void generate();
    
    /**
     * @brief Get all cell data for serialization
     * @return Vector of cell information
     */
    std::vector<uint8_t> serialize() const;
    
    /**
     * @brief Load board from serialized data
     * @param data Serialized board data
     */
    void deserialize(std::vector<uint8_t> const& data);

private:
    std::vector<std::vector<Cell>> grid_;
    std::mt19937 rng_;
    uint32_t seed_;
    std::size_t grid_size_;
    
    void generate_blocked_cells();
    void generate_special_cells();
    void generate_number_cells();
};

}
}

#endif // MINIMAX_DOMAIN_BOARD_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
