#include "cerebro/async_logger.hpp"

#include <chrono>
#include <ctime>
#include <sstream>
#include <utility>

namespace cerebro {

AsyncLogger::AsyncLogger() {
    start();
}

AsyncLogger::~AsyncLogger() {
    stop();
}

void AsyncLogger::start() {
    bool expected = false;
    if (running_.compare_exchange_strong(expected, true)) {
        worker_ = std::thread(&AsyncLogger::run, this);
    }
}

void AsyncLogger::stop() {
    if (running_.exchange(false)) {
        queue_.stop();
        if (worker_.joinable()) {
            worker_.join();
        }
    }
}

void AsyncLogger::log(LogLevel level, std::string message) {
    queue_.push(LogEntry{std::chrono::system_clock::now(), level, std::move(message)});
}

std::string_view AsyncLogger::level_name(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
    }
    return "INFO";
}

std::string AsyncLogger::format_timestamp(std::chrono::system_clock::time_point timestamp) {
    const auto current_time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm time_info{};
#if defined(_WIN32)
    localtime_s(&time_info, &current_time);
#else
    localtime_r(&current_time, &time_info);
#endif
    std::ostringstream out;
    out << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

void AsyncLogger::run() {
    while (running_) {
        const auto entry = queue_.wait_and_pop();
        if (!running_ && entry.message.empty()) {
            break;
        }
        if (!entry.message.empty()) {
            std::cout << '[' << format_timestamp(entry.timestamp) << "] " << level_name(entry.level) << " " << entry.message << '\n';
        }
    }
}

} // namespace cerebro
