#include "minimax/domain/game_state.hpp"

namespace minimax {
namespace domain {

GameState::GameState()
: current_player_(Player::PLAYER1)
, player1_score_(0)
, player2_score_(0)
, constraint_(Constraint::NONE)
, constraint_index_(0)
, move_number_(0)
, game_over_(false)
, result_(GameResult::IN_PROGRESS)
{
}

Player GameState::get_current_player() const
{
    return current_player_;
}

void GameState::switch_player()
{
    current_player_ = (current_player_ == Player::PLAYER1) ? Player::PLAYER2 : Player::PLAYER1;
}

int GameState::get_score(Player player) const
{
    return (player == Player::PLAYER1) ? player1_score_ : player2_score_;
}

void GameState::set_score(Player player, int score)
{
    if (player == Player::PLAYER1) {
        player1_score_ = score;
    } else {
        player2_score_ = score;
    }
}

void GameState::add_score(Player player, int points)
{
    if (player == Player::PLAYER1) {
        player1_score_ += points;
    } else {
        player2_score_ += points;
    }
}

Constraint GameState::get_constraint() const
{
    return constraint_;
}

void GameState::set_constraint(Constraint constraint)
{
    constraint_ = constraint;
}

std::size_t GameState::get_constraint_index() const
{
    return constraint_index_;
}

void GameState::set_constraint_index(std::size_t index)
{
    constraint_index_ = index;
}

std::size_t GameState::get_move_number() const
{
    return move_number_;
}

void GameState::increment_move()
{
    ++move_number_;
}

bool GameState::is_game_over() const
{
    return game_over_;
}

void GameState::set_game_over(bool over)
{
    game_over_ = over;
}

GameResult GameState::get_result() const
{
    return result_;
}

void GameState::calculate_result()
{
    if (player1_score_ > player2_score_) {
        result_ = GameResult::PLAYER1_WIN;
    } else if (player1_score_ < player2_score_) {
        result_ = GameResult::PLAYER2_WIN;
    } else {
        result_ = GameResult::DRAW;
    }
}

}
}
