#include "minimax/domain/score_engine.hpp"

namespace minimax {
namespace domain {

ScoreEngine::ScoreEngine(GameState& state)
: state_(state)
{
}

void ScoreEngine::apply_cell_score(Player player, const Cell& cell)
{
    CellType type = cell.get_type();
    
    switch (type) {
        case CellType::NUMBER:
            apply_number_cell(player, cell.get_value());
            break;
        case CellType::BOMB:
            apply_bomb_cell(player);
            break;
        case CellType::SURPRISE:
            apply_surprise_cell(player, cell.get_bonus());
            break;
        default:
            break;
    }
}

void ScoreEngine::apply_number_cell(Player player, int value)
{
    state_.add_score(player, value);
}

void ScoreEngine::apply_bomb_cell(Player player)
{
    int current_score = state_.get_score(player);
    state_.set_score(player, current_score / 2);
}

void ScoreEngine::apply_surprise_cell(Player player, SurpriseBonus bonus)
{
    int current_score = state_.get_score(player);
    int bonus_percent = static_cast<int>(bonus);
    int bonus_points = (current_score * bonus_percent) / 100;
    state_.add_score(player, bonus_points);
}

}
}
