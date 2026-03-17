#include "minimax/common/logging.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace minimax {
namespace logging {

const std::unordered_map<Level, std::string> level_names = {
    {Level::INFO, "INFO"},
    {Level::WARNING, "WARNING"},
    {Level::ERROR, "ERROR"}
};

std::ostream& Logger::output_stream_ = std::cout;

void Logger::log(Level level, std::string const& message)
{    
    output_stream_ << "[" << get_timestamp() << "] "
              << level_to_string(level) << ": "
              << message << std::endl;
}

std::string Logger::level_to_string(Level level) {
    auto it = level_names.find(level);
    if (it != level_names.end()) {
        return it->second;
    }
    
    return "UNKNOWN";
}

std::string Logger::get_timestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

}
}
