#ifndef MINIMAX_DOMAIN_SCORE_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_SCORE_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include "minimax/domain/cell.hpp"
#include "minimax/domain/game_state.hpp"

namespace minimax {
namespace domain {

class ScoreEngine {
public:
    explicit ScoreEngine(GameState& state);
    
    ~ScoreEngine() = default;
    ScoreEngine(const ScoreEngine& other) = delete;
    ScoreEngine& operator=(const ScoreEngine& other) = delete;
    
    void apply_cell_score(Player player, const Cell& cell);

private:
    GameState& state_;
    
    void apply_number_cell(Player player, int value);
    void apply_bomb_cell(Player player);
    void apply_surprise_cell(Player player, SurpriseBonus bonus);
};

}
}

#endif // MINIMAX_DOMAIN_SCORE_ENGINE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
