#ifndef MINIMAX_COMMON_TYPES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_COMMON_TYPES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include <cstddef>
#include <cstdint>
#include <string>

namespace minimax {

constexpr std::size_t GRID_SIZE = 10;
constexpr std::size_t MAX_BLOCKED_CELLS = 4;
constexpr std::size_t DEFAULT_SEED = 42;
constexpr std::size_t DEFAULT_PORT = 5555;
const std::string LOCALHOST = "127.0.0.1";

constexpr int MIN_CELL_VALUE = -10;
constexpr int MAX_CELL_VALUE = 42;
constexpr int BOMBS_NUM = 2;
constexpr int SURPRISES_NUM = 7;

constexpr auto CLEAR_SCREEN = "\033[2J\033[H";

constexpr auto YELLOW = "\033[43m\033[30m";
constexpr auto GREEN = "\033[42m\033[30m";
constexpr auto BLUE = "\033[44m\033[37m";
constexpr auto GRAY = "\033[90m";
constexpr auto WHITE = "\033[47m\033[30m";
constexpr auto NO_BACKGROUND = "\033[0m";

constexpr auto GREEN_FONT = "\033[32m";
constexpr auto BLUE_FONT = "\033[34m";
constexpr auto RESET_COLOR = "\033[0m";

enum class Player : uint8_t {
    PLAYER1 = 1,
    PLAYER2 = 2,
    NONE = 0
};

enum class CellType : uint8_t {
    NUMBER = 0,
    BLOCKED = 1,
    BOMB = 2,
    SURPRISE = 3
};

enum class Constraint : uint8_t {
    NONE = 0,
    ROW = 1,
    COLUMN = 2
};

enum class SurpriseBonus : uint8_t {
    BONUS_20 = 20,
    BONUS_50 = 50
};

enum class GameResult : uint8_t {
    PLAYER1_WIN,
    PLAYER2_WIN,
    DRAW,
    IN_PROGRESS
};

struct Position {
    std::size_t row;
    std::size_t col;
    
    bool operator==(Position const& other) const
    {
        return row == other.row && col == other.col;
    }
    
    bool operator!=(Position const& other) const
    {
        return !(*this == other);
    }
};

}

#endif // MINIMAX_COMMON_TYPES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
