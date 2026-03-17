#include "minimax/domain/rules_engine.hpp"

namespace minimax {
namespace domain {

RulesEngine::RulesEngine(Board& board, GameState& state)
: board_(board)
, state_(state)
{
}

bool RulesEngine::is_legal_move(Position pos) const
{
    if (!board_.is_valid_position(pos)) {
        return false;
    }
    
    if (!is_cell_available(pos)) {
        return false;
    }
    
    return satisfies_constraint(pos);
}

std::vector<Position> RulesEngine::get_legal_moves() const
{
    std::vector<Position> moves;
    
    for (std::size_t i = 0; i < GRID_SIZE; ++i) {
        for (std::size_t j = 0; j < GRID_SIZE; ++j) {
            Position pos{i, j};
            if (is_legal_move(pos)) {
                moves.push_back(pos);
            }
        }
    }
    
    return moves;
}

bool RulesEngine::has_legal_moves() const
{
    for (std::size_t i = 0; i < GRID_SIZE; ++i) {
        for (std::size_t j = 0; j < GRID_SIZE; ++j) {
            if (is_legal_move({i, j})) {
                return true;
            }
        }
    }
    return false;
}

bool RulesEngine::satisfies_constraint(Position pos) const
{
    Constraint constraint = state_.get_constraint();
    
    if (constraint == Constraint::NONE) {
        return true;
    }
    
    std::size_t constraint_idx = state_.get_constraint_index();
    
    if (constraint == Constraint::ROW) {
        return pos.row == constraint_idx;
    } else if (constraint == Constraint::COLUMN) {
        return pos.col == constraint_idx;
    }
    
    return false;
}

bool RulesEngine::is_cell_available(Position pos) const
{
    Cell const& cell = board_.get_cell(pos);
    return cell.is_selectable();
}

}
}
