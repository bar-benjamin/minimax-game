#ifndef APP_GAME_CLIENT_APP_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define APP_GAME_CLIENT_APP_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/domain/game_controller.hpp"
#include "minimax/net/transport.hpp"
#include "minimax/ui/renderer.hpp"
#include <memory>
#include <string>
#include <atomic>

namespace minimax {
namespace app {

class GameClientApp {
public:
    explicit GameClientApp(bool is_host, const std::string& addr, uint16_t port,
                           unsigned int seed=DEFAULT_SEED, std::size_t grid_size=GRID_SIZE);
    ~GameClientApp();
    
    void run();

private:
    bool is_server_;
    std::string ip_address_;
    uint16_t port_;
    std::unique_ptr<domain::GameController> controller_;
    std::unique_ptr<net::Transport> transport_;
    std::unique_ptr<ui::Renderer> renderer_;
    std::atomic<bool> running_;
    
    bool setup_connection();
    void game_loop();
    void handle_input();
    Position get_move_from_user();

    void send_board_to_host();
    void receive_board_from_host();

    void send_move_to_opponent(Position pos);
    Position receive_move_from_opponent();
};

}
}

#endif // APP_GAME_CLIENT_APP_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
