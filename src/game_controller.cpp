#include "minimax/domain/game_controller.hpp"

namespace minimax {
namespace domain {

GameController::GameController(unsigned int seed, std::size_t grid_size)
: board_(std::make_unique<Board>(seed, grid_size))
, state_(std::make_unique<GameState>())
, rules_(nullptr)
, score_(nullptr)
{
    rules_ = std::make_unique<RulesEngine>(*board_, *state_);
    score_ = std::make_unique<ScoreEngine>(*state_);
}

void GameController::initialize_game()
{
    board_->generate();
    state_ = std::make_unique<GameState>();
    rules_ = std::make_unique<RulesEngine>(*board_, *state_);
    score_ = std::make_unique<ScoreEngine>(*state_);
}

bool GameController::process_move(Position pos)
{
    if (!rules_->is_legal_move(pos)) {
        return false;
    }
    
    Cell& cell = board_->get_cell(pos);
    Player current = state_->get_current_player();
    
    score_->apply_cell_score(current, cell);
    cell.consume(current);
    
    update_constraint_after_move(pos);
    
    state_->increment_move();
    state_->switch_player();
    
    return true;
}

const Board& GameController::get_board() const
{
    return *board_;
}

const GameState& GameController::get_state() const
{
    return *state_;
}

const RulesEngine& GameController::get_rules() const
{
    return *rules_;
}

void GameController::load_board(std::vector<uint8_t> const& data)
{
    board_->deserialize(data);
}

bool GameController::check_game_end()
{
    if (!rules_->has_legal_moves()) {

        state_->set_game_over(true);
        state_->calculate_result();
        return true;
    }

    return false;
}

void GameController::update_constraint_after_move(Position pos)
{
    std::size_t move_num = state_->get_move_number();
    
    if (move_num == 0) {
        // First move: Player 2 must select from same COLUMN
        state_->set_constraint(Constraint::COLUMN);
        state_->set_constraint_index(pos.col);
    } else if (state_->get_current_player() == Player::PLAYER1) {
        // Player 1 moved: Player 2 must select from same COLUMN
        state_->set_constraint(Constraint::COLUMN);
        state_->set_constraint_index(pos.col);
    } else {
        // Player 2 moved: Player 1 must select from same ROW
        state_->set_constraint(Constraint::ROW);
        state_->set_constraint_index(pos.row);
    }
}

}
}