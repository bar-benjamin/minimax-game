#include "minimax/ui/input.hpp"
#include <unistd.h>
#include <cstdio>

namespace minimax {
namespace ui {

namespace {
    constexpr char ESC = '\033';
    constexpr char SPACE = ' ';
}

Input::Input()
    : m_raw_mode_enabled(false)
{
    enable_raw_mode();
}

Input::~Input()
{
    restore_terminal();
}

void Input::enable_raw_mode()
{
    if (m_raw_mode_enabled) {
        return;
    }

    tcgetattr(STDIN_FILENO, &m_old_termios);

    termios raw = m_old_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    m_raw_mode_enabled = true;
}

void Input::restore_terminal()
{
    if (!m_raw_mode_enabled) {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &m_old_termios);
    m_raw_mode_enabled = false;
}

InputCommand Input::read_command()
{
    char key = getchar();

    if (key == 'q' || key == 'Q') {
        return InputCommand::QUIT;
    }

    if (key == SPACE) {
        return InputCommand::SELECT;
    }

    if (key == ESC) {
        char second = getchar();
        if (second == '[') {
            char arrow = getchar();
            switch (arrow) {
            case 'A': // ↑ Up
                return InputCommand::MOVE_UP;
            case 'B': // ↓ Down
                return InputCommand::MOVE_DOWN;
            case 'C': // -> Right
                return InputCommand::MOVE_RIGHT;
            case 'D': // <- Left
                return InputCommand::MOVE_LEFT;
            default:
                return InputCommand::NONE;
            }
        }
    }

    return InputCommand::NONE;
}

}
}
