#ifndef MINIMAX_DOMAIN_GAME_CONTROLLER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_GAME_CONTROLLER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include "minimax/domain/board.hpp"
#include "minimax/domain/game_state.hpp"
#include "minimax/domain/rules_engine.hpp"
#include "minimax/domain/score_engine.hpp"
#include <memory>
#include <functional>

namespace minimax {
namespace domain {

/**
 * Orchestrating game with all of its components
 */
class GameController {
public:
    explicit GameController(unsigned int seed=DEFAULT_SEED, std::size_t grid_size=GRID_SIZE);
    
    ~GameController() = default;
    GameController(const GameController& other) = delete;
    GameController& operator=(const GameController& other) = delete;
    
    void initialize_game();

    bool process_move(Position pos);    
    
    const Board& get_board() const;    
    const GameState& get_state() const;    
    const RulesEngine& get_rules() const;
    
    void load_board(const std::vector<uint8_t>& data);
    bool check_game_end();

private:
    std::unique_ptr<Board> board_;
    std::unique_ptr<GameState> state_;
    std::unique_ptr<RulesEngine> rules_;
    std::unique_ptr<ScoreEngine> score_;
    
    void update_constraint_after_move(Position pos);
};

}
}

#endif // MINIMAX_DOMAIN_GAME_CONTROLLER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
