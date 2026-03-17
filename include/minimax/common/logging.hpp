#ifndef MINIMAX_COMMON_LOGGING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_COMMON_LOGGING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

namespace minimax {
namespace logging {

enum class Level {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    /**
     * @brief Log a message by min_level_ filter for logging control
     */
    static void log(Level level, const std::string& message);
    
private:
    static std::ostream& output_stream_;
    static std::string level_to_string(Level level);
    static std::string get_timestamp();
};

}
}

#endif // MINIMAX_COMMON_LOGGING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
