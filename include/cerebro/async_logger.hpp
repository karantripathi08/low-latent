#pragma once

#include "cerebro/thread_safe_queue.hpp"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>

namespace cerebro {

enum class LogLevel { Debug, Info, Warning, Error };

struct LogEntry {
    std::chrono::system_clock::time_point timestamp{};
    LogLevel level{LogLevel::Info};
    std::string message;
};

class AsyncLogger {
public:
    AsyncLogger();
    ~AsyncLogger();

    void start();
    void stop();
    void log(LogLevel level, std::string message);

private:
    static std::string_view level_name(LogLevel level);
    static std::string format_timestamp(std::chrono::system_clock::time_point timestamp);
    void run();

    ThreadSafeQueue<LogEntry> queue_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace cerebro
