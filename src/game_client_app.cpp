#include "minimax/app/game_client_app.hpp"
#include "minimax/net/socket.hpp"
#include "minimax/net/transport_blocking.hpp"
#include "minimax/ui/input.hpp"
#include "minimax/common/logging.hpp"
#include <iostream>

namespace minimax {
namespace app {

GameClientApp::GameClientApp(bool is_host, const std::string& addr, uint16_t port,
                             unsigned int seed, std::size_t grid_size)
: is_server_(is_host)
, ip_address_(addr)
, port_(port)
, controller_(std::make_unique<minimax::domain::GameController>(seed, grid_size))
, transport_(nullptr)
, renderer_(std::make_unique<minimax::ui::Renderer>())
, running_(true)
{
    logging::Logger::log(logging::Level::INFO, "App created: " + 
                         std::string(is_server_ ? "SERVER" : "CLIENT"));
}

GameClientApp::~GameClientApp()
{
    running_ = false;
    if (transport_) {
        transport_->disconnect();
    }
}

void GameClientApp::run()
{
    logging::Logger::log(logging::Level::INFO, "Starting game...");
    
    if (!setup_connection()) {
        logging::Logger::log(logging::Level::ERROR, "Connection Failed");
        return;
    }
    
    controller_->initialize_game();
    
    if (is_server_) {
        send_board_to_host();
    } else {
        receive_board_from_host();
    }
    
    game_loop();
}

bool GameClientApp::setup_connection()
{    
    try {
        if (is_server_) {
            logging::Logger::log(logging::Level::INFO, "Starting server on port " + std::to_string(port_));
            
            net::Socket server_socket = net::Socket::create_and_bind(port_);
            server_socket.listen();
            
            logging::Logger::log(logging::Level::INFO, "Waiting for client...");
            
            net::Socket client_socket = server_socket.accept();

            logging::Logger::log(logging::Level::INFO, "Client connected!");
            
            transport_ = std::make_unique<net::BlockingTransport>(std::move(client_socket));
            
        } else { // is client
            logging::Logger::log(logging::Level::INFO, "Connecting to " + ip_address_ + ":" + std::to_string(port_));
            
            net::Socket socket = net::Socket::create_tcp();
            
            if (!socket.connect(ip_address_, port_)) {
                return false;
            }
            
            logging::Logger::log(logging::Level::INFO, "Connected!");
            transport_ = std::make_unique<net::BlockingTransport>(std::move(socket));
        }
        
        return true;
    } catch (const std::exception& e) {
        logging::Logger::log(logging::Level::ERROR, std::string("Connection error: ") + e.what());
        return false;
    }
}

void GameClientApp::game_loop()
{
    minimax::Position cursor{0, 0};
    minimax::ui::Input input;
    
    Player my_player = is_server_ ? Player::PLAYER1 : Player::PLAYER2;
    
    while (running_ && !controller_->get_state().is_game_over()) {
        renderer_->render_board(controller_->get_board(), controller_->get_state(), cursor);
        
        Player current = controller_->get_state().get_current_player();
        
        if (current == my_player) {
            Position pos = get_move_from_user();
            
            if (controller_->process_move(pos)) {
                send_move_to_opponent(pos);
                controller_->check_game_end();
            }
        } else { // opponent's turn
            Position opp_move = receive_move_from_opponent();
            controller_->process_move(opp_move);
            controller_->check_game_end();
        }
    }
    
    renderer_->render_game_over(controller_->get_state());
}

Position GameClientApp::get_move_from_user()
{
    Position cursor{0, 0};
    ui::Input input;
    
    while (true) {
        renderer_->render_board(controller_->get_board(),
                               controller_->get_state(),
                               cursor);
        
        ui::InputCommand cmd = input.read_command();
        
        switch (cmd) {
            case ui::InputCommand::MOVE_UP:
                if (cursor.row > 0) {
                    cursor.row--;
                }
                break;
            case ui::InputCommand::MOVE_DOWN:
                if (cursor.row < GRID_SIZE - 1) {
                    cursor.row++;
                }
                break;
            case ui::InputCommand::MOVE_LEFT:
                if (cursor.col > 0) {
                    cursor.col--;
                }
                break;
            case ui::InputCommand::MOVE_RIGHT:
                if (cursor.col < GRID_SIZE - 1) {
                    cursor.col++;
                }
                break;
            case ui::InputCommand::SELECT:
                return cursor;
            case ui::InputCommand::QUIT:
                running_ = false;
                return {0, 0};
            default:
                break;
        }
    }
}

void GameClientApp::send_board_to_host()
{
    logging::Logger::log(logging::Level::INFO, "Send board to host...");
    
    try {
        std::vector<uint8_t> board_data = controller_->get_board().serialize();
        
        std::vector<uint8_t> message;
        message.reserve(1 + board_data.size()); // message_type(1 byte) + board_data
        message.push_back(static_cast<uint8_t>(minimax::protocol::MessageType::GRID));
        message.insert(message.end(), board_data.begin(), board_data.end());
        
        if (!transport_->send_message(message)) {
            running_ = false;
            throw std::runtime_error("Failed to send board to host");
        }
        
        logging::Logger::log(logging::Level::INFO, "Board sent successfully!");
    } catch (const std::exception& e) {
        logging::Logger::log(logging::Level::ERROR, std::string("Error sending board: ") + e.what());
        running_ = false;
        throw;
    }
}

void GameClientApp::receive_board_from_host()
{
    logging::Logger::log(logging::Level::INFO, "Receive board from host...");

    try {
        std::vector<uint8_t> message;
        if (!transport_->receive_message(message)) {
            running_ = false;
            throw std::runtime_error("Failed to receive board from host");
        }
        
        if (message.empty()) {
            running_ = false;
            throw std::runtime_error("Empty board message");
        }
        
        uint8_t msg_type = message[0];
        if (msg_type != static_cast<uint8_t>(minimax::protocol::MessageType::GRID)) {
            running_ = false;
            throw std::runtime_error("Invalid message type");
        }
        
        // Skip message type byte
        std::vector<uint8_t> board_data(message.begin() + 1, message.end());
        
        controller_->load_board(board_data);
        
        logging::Logger::log(logging::Level::INFO, "Board received successfully!");
    } catch (const std::exception& e) {
        logging::Logger::log(logging::Level::ERROR, std::string("Error receiving board: ") + e.what());
        running_ = false;
        throw;
    }
}

void GameClientApp::send_move_to_opponent(Position pos)
{
    logging::Logger::log(logging::Level::INFO, "Sending move to opponent: (" + std::to_string(pos.row) + ", " + 
                                                                               std::to_string(pos.col) + ")");

    try {
        std::vector<uint8_t> message;
        message.reserve(3); // message_type(1 byte) + row(1 byte) + col(1 byte)
        message.push_back(static_cast<uint8_t>(minimax::protocol::MessageType::MOVE));
        message.push_back(static_cast<uint8_t>(pos.row));
        message.push_back(static_cast<uint8_t>(pos.col));
        
        if (!transport_->send_message(message)) {
            running_ = false;
            throw std::runtime_error("Failed to send move");
        }
        
        logging::Logger::log(logging::Level::INFO, "Move sent successfully!");
    } catch (const std::exception& e) {
        running_ = false;
        throw;
    }
}

Position GameClientApp::receive_move_from_opponent()
{
    logging::Logger::log(logging::Level::INFO, "Waiting for opponent's move...");
    
    try {
        std::vector<uint8_t> message;
        if (!transport_->receive_message(message)) {
            running_ = false;
            throw std::runtime_error("Failed to receive move");
        }
        
        if (message.size() != 3) {
            running_ = false;
            throw std::runtime_error("Invalid move message size");
        }
        
        uint8_t msg_type = message[0];
        if (msg_type != static_cast<uint8_t>(minimax::protocol::MessageType::MOVE)) {
            running_ = false;
            throw std::runtime_error("Invalid message type");
        }
        
        Position pos;
        pos.row = static_cast<std::size_t>(message[1]);
        pos.col = static_cast<std::size_t>(message[2]);
        
        if (pos.row >= GRID_SIZE || pos.col >= GRID_SIZE) {
            running_ = false;
            throw std::runtime_error("Invalid move position");
        }

        logging::Logger::log(logging::Level::INFO, "Received opponent's move: (" + 
                                                   std::to_string(pos.row) + ", " + 
                                                   std::to_string(pos.col) + ")");

        return pos;
        
    } catch (const std::exception& e) {
        running_ = false;
        throw;
    }
}

}
}
