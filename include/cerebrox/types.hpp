#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace cerebrox {

enum class ProcessClass {
    Interactive,
    Game,
    Background,
    Server,
    Unknown
};

enum class Recommendation {
    ProtectForeground,
    IncreasePriority,
    DecreasePriority,
    PinToCore,
    MoveToBackground,
    BatchIOTasks,
    NoChange
};

struct ProcessSample {
    std::uint32_t pid{};
    std::string name;
    double cpu_percent{};
    double memory_mb{};
    std::uint32_t threads{};
    std::uint64_t context_switches{};
    std::uint64_t io_bytes_per_sec{};
    bool foreground{false};
    std::chrono::steady_clock::time_point timestamp{};
};

struct ProcessState {
    ProcessSample latest;
    std::vector<ProcessSample> history;
};

struct WorkloadPrediction {
    ProcessClass classification{ProcessClass::Unknown};
    double predicted_cpu_percent{};
    double burst_score{};
    double confidence{};
    Recommendation recommendation{Recommendation::NoChange};
    std::string rationale;
};

struct Decision {
    std::uint32_t pid{};
    std::string process_name;
    ProcessClass classification{ProcessClass::Unknown};
    double predicted_cpu_percent{};
    double burst_score{};
    double confidence{};
    Recommendation recommendation{Recommendation::NoChange};
    std::string rationale;
    std::chrono::nanoseconds latency{};
};

} // namespace cerebrox
