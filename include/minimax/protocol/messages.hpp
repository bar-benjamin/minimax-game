#ifndef MINIMAX_PROTOCOL_MESSAGES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_PROTOCOL_MESSAGES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace minimax {
namespace protocol {

enum class MessageType : uint8_t {
    HELLO = 1,
    WELCOME = 2,
    GRID = 3,
    STATE = 4,
    MOVE = 5,
    MOVE_RESULT = 6,
    GAME_OVER = 7,
    ERROR = 8,
};

struct Message {
    MessageType type;
    virtual ~Message() = default;
protected:
    explicit Message(MessageType t) : type(t) {}
};

struct HelloMessage : public Message {
    std::string nickname;
    uint16_t protocol_version;

    explicit HelloMessage(const std::string& n = "", uint16_t version = 1)
    : Message(MessageType::HELLO), nickname(n), protocol_version(version) {}
};

struct GridMessage : public Message {
    std::vector<uint8_t> grid_data;

    explicit GridMessage(const std::vector<uint8_t>& data = {})
    : Message(MessageType::GRID)
    , grid_data(data) {}
};

struct StateMessage : public Message {
    int player1_score;
    int player2_score;
    uint8_t current_player;
    uint8_t constraint_type;
    
    explicit StateMessage()
    : Message(MessageType::STATE)
    , player1_score(0)
    , player2_score(0)
    , current_player(1)
    , constraint_type(0) {}
};

struct MoveMessage : public Message {
    std::size_t row;
    std::size_t col;

    explicit MoveMessage(std::size_t r = 0, std::size_t c = 0)
    : Message(MessageType::MOVE)
    , row(r)
    , col(c) {}
};

struct MoveResultMessage : public Message {
    bool accepted;
    std::string reason;
    int updated_score;
    explicit MoveResultMessage(bool accepted = true, const std::string& reason = "", int updated_score = 0)
    : Message(MessageType::MOVE_RESULT)
    , accepted(accepted)
    , reason(reason)
    , updated_score(updated_score) {}
};

struct GameOverMessage : public Message {
    int player1_final_score;
    int player2_final_score;
    uint8_t game_result;

    explicit GameOverMessage()
    : Message(MessageType::GAME_OVER)
    , player1_final_score(0)
    , player2_final_score(0)
    , game_result(static_cast<uint8_t>(GameResult::DRAW)) {}
};

struct ErrorMessage : public Message {
    std::string error_violation;

    explicit ErrorMessage(const std::string& error_violation = "")
    : Message(MessageType::ERROR)
    , error_violation(error_violation) {}
};

}
}

#endif // MINIMAX_PROTOCOL_MESSAGES_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
