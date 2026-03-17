#include "minimax/ui/renderer.hpp"
#include <iostream>
#include <iomanip>

namespace minimax {
namespace ui {

Renderer::Renderer(std::ostream& output_stream)
    : output_stream_(output_stream)
{
}

void Renderer::clear_screen() const
{
    output_stream_ << CLEAR_SCREEN << std::flush;
}

void Renderer::render_board(const minimax::domain::Board& board,
                            const minimax::domain::GameState& state,
                            minimax::Position cursor) const
{    
    clear_screen();
    
    output_stream_ << "MINIMAX GAME - Player " 
        << static_cast<int>(state.get_current_player()) << " Turn\n\n";
    
    output_stream_ << GREEN_FONT << "Player 1 Score: " << state.get_score(Player::PLAYER1) << RESET_COLOR << "  |  "
        << BLUE_FONT << "Player 2 Score: " << state.get_score(Player::PLAYER2) << RESET_COLOR << "\n\n";
    
    Constraint constraint = state.get_constraint();
    if (constraint != Constraint::NONE) {
        output_stream_ << "Constraint: ";
        if (constraint == Constraint::ROW) {
            output_stream_ << "Must select from ROW " << state.get_constraint_index();
        } else {
            output_stream_ << "Must select from COLUMN " << state.get_constraint_index();
        }
        output_stream_ << "\n\n";
    }
    
    output_stream_ << "   ";
    for (std::size_t col = 0; col < GRID_SIZE; ++col) {
        output_stream_ << std::setw(5) << std::right << col;
    }
    output_stream_ << "\n";
    
    for (std::size_t row = 0; row < GRID_SIZE; ++row) {
        output_stream_ << std::setw(2) << row << " ";
        
        for (std::size_t col = 0; col < GRID_SIZE; ++col) {
            Position pos{row, col};
            const domain::Cell& cell = board.get_cell(pos);
            
            bool is_constrained = false;
            if ((constraint == Constraint::ROW && row == state.get_constraint_index()) || 
                (constraint == Constraint::COLUMN && col == state.get_constraint_index())) {
                is_constrained = true;
            }
            
            print_cell(cell, pos, cursor, is_constrained);
        }
        output_stream_ << "\n";
    }
    
    output_stream_ << "\nControls: Arrow Keys = Move | Space = Select | Q = Quit\n";
}

void Renderer::render_game_over(const minimax::domain::GameState& state) const
{    
    clear_screen();
    
    output_stream_ << "\n=== GAME OVER ===\n\n";
    output_stream_ << "Player 1 Final Score: " << state.get_score(Player::PLAYER1) << "\n";
    output_stream_ << "Player 2 Final Score: " << state.get_score(Player::PLAYER2) << "\n\n";
    
    GameResult result = state.get_result();
    if (result == GameResult::PLAYER1_WIN) {
        output_stream_ << "Winner: Player 1!\n";
    } else if (result == GameResult::PLAYER2_WIN) {
        output_stream_ << "Winner: Player 2!\n";
    } else {
        output_stream_ << "Result: Draw!\n";
    }
}

void Renderer::print_cell(minimax::domain::Cell const& cell,
                         minimax::Position pos,
                         minimax::Position cursor,
                         bool is_constrained) const
{
    bool is_cursor = (pos == cursor);
    std::string color = get_color_code(cell, is_cursor, is_constrained);
    std::string display = get_cell_display(cell);
    
    bool has_emoji = (cell.get_type() == minimax::CellType::BLOCKED ||
                      cell.get_type() == minimax::CellType::BOMB ||
                      cell.get_type() == minimax::CellType::SURPRISE);
    
    if (has_emoji) {
        output_stream_ << color << "   " << display << RESET_COLOR;
    } else {
        output_stream_ << color << std::setw(5) << std::right << display << RESET_COLOR;
    }
}

std::string Renderer::get_cell_display(const minimax::domain::Cell& cell) const
{
    using namespace minimax;
    
    switch (cell.get_type()) {
        case CellType::NUMBER:
            return std::to_string(cell.get_value());
        case CellType::BLOCKED:
            return "🚧";
        case CellType::BOMB:
            return "💣";
        case CellType::SURPRISE:
            return "🎉";
        default:
            return "?";
    }
}

std::string Renderer::get_color_code(minimax::domain::Cell const& cell,
                                    bool is_cursor,
                                    bool is_constrained) const
{
    using namespace minimax;
    
    if (is_cursor) {
        return YELLOW;  
    }
    
    if (cell.is_consumed()) {
        Player consumed_by = cell.get_consumed_by();
        if (consumed_by == Player::PLAYER1) {
            return GREEN;
        } else if (consumed_by == Player::PLAYER2) {
            return BLUE;
        }
        return "\0";
    }
    
    if (cell.get_type() == CellType::BLOCKED) {
        return GRAY;
    }
    
    // Legal cells to choose from
    if (is_constrained) {
        return WHITE;
    }
    
    return NO_BACKGROUND;
}

}
}
