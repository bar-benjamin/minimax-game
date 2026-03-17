#ifndef MINIMAX_UI_INPUT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_UI_INPUT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include <termios.h>

namespace minimax {
namespace ui {

enum class InputCommand {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    SELECT,
    QUIT,
    NONE
};

class Input {
public:
    Input();

    ~Input();
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    InputCommand read_command();

private:
    termios m_old_termios;
    bool m_raw_mode_enabled;

    void enable_raw_mode();
    void restore_terminal();
};

}
}

#endif // MINIMAX_UI_INPUT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e