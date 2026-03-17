#ifndef MINIMAX_DOMAIN_RULES_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_RULES_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include "minimax/domain/board.hpp"
#include "minimax/domain/game_state.hpp"
#include <vector>

namespace minimax {
namespace domain {

class RulesEngine {
public:
    explicit RulesEngine(Board& board, GameState& state);
    
    ~RulesEngine() = default;
    RulesEngine(const RulesEngine& other) = delete;
    RulesEngine& operator=(const RulesEngine& other) = delete;
    
    bool is_legal_move(Position pos) const;
    std::vector<Position> get_legal_moves() const;
    bool has_legal_moves() const;

private:
    Board& board_;
    GameState& state_;
    
    bool satisfies_constraint(Position pos) const;
    bool is_cell_available(Position pos) const;
};

}
}

#endif // MINIMAX_DOMAIN_RULES_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
