#ifndef UI_RENDERER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define UI_RENDERER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/domain/board.hpp"
#include "minimax/domain/game_state.hpp"
#include "minimax/common/types.hpp"

#include <iostream>

namespace minimax {
namespace ui {

class Renderer {
public:
    explicit Renderer(std::ostream& output_stream = std::cout);
    
    void render_board(const minimax::domain::Board& board,
                      const minimax::domain::GameState& state,
                      minimax::Position cursor) const;
    
    void render_game_over(const minimax::domain::GameState& state) const;
    
    void clear_screen() const;

private:
    std::ostream& output_stream_;

    void print_cell(const minimax::domain::Cell& cell,
                   minimax::Position pos,
                   minimax::Position cursor,
                   bool is_constrained) const;
    std::string get_cell_display(const minimax::domain::Cell& cell) const;
    std::string get_color_code(const minimax::domain::Cell& cell,
                              bool is_cursor,
                              bool is_constrained) const;
};

}
}

#endif // UI_RENDERER_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
