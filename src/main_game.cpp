#include "minimax/domain/game_controller.hpp"
#include "minimax/ui/renderer.hpp"
#include "minimax/ui/input.hpp"
#include "minimax/common/logging.hpp"
#include "minimax/app/game_client_app.hpp"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[])
{
    uint32_t seed = minimax::DEFAULT_SEED;
    uint16_t port = minimax::DEFAULT_PORT;
    std::size_t grid_size = minimax::GRID_SIZE;
    std::string addr = minimax::LOCALHOST;
    std::string mode = "host";

    minimax::logging::Logger::log(minimax::logging::Level::INFO, "Starting Minimax Game..");

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            mode = argv[++i];
        } else if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (std::strcmp(argv[i], "--addr") == 0 && i + 1 < argc) {
            addr = argv[++i];
        } else if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = static_cast<uint16_t>(std::stoul(argv[++i]));
        }
    }

    try {
        bool is_host = (mode == "host");
        minimax::app::GameClientApp app(is_host, addr, port, seed, grid_size);
        app.run();

        minimax::logging::Logger::log(minimax::logging::Level::INFO, "GAME OVER ^_^");
        
    } catch (const std::exception& e) {
        minimax::logging::Logger::log(minimax::logging::Level::ERROR, std::string("Exception: ") + e.what());
        return 1;
    }

    return 0;
}
