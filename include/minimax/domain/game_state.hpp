#ifndef MINIMAX_DOMAIN_GAME_STATE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_GAME_STATE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include "minimax/domain/board.hpp"

namespace minimax {
namespace domain {

class GameState {
public:
    explicit GameState();
    
    ~GameState() = default;
    GameState(const GameState& other) = default;
    GameState& operator=(const GameState& other) = default;

    Player get_current_player() const;
    void switch_player();
    
    int get_score(Player player) const;    
    void set_score(Player player, int score);    
    void add_score(Player player, int points);
    
    Constraint get_constraint() const;
    void set_constraint(Constraint constraint);    
    std::size_t get_constraint_index() const;    
    void set_constraint_index(std::size_t index);
    
    std::size_t get_move_number() const;    
    void increment_move();

    bool is_game_over() const;    
    void set_game_over(bool over);
        
    GameResult get_result() const;    
    void calculate_result();

private:
    Player current_player_;
    int player1_score_;
    int player2_score_;
    Constraint constraint_;
    std::size_t constraint_index_;
    std::size_t move_number_;
    bool game_over_;
    GameResult result_;
};

}
}

#endif
