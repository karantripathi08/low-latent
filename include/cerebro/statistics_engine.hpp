#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace cerebro {

struct StatisticsSnapshot {
    std::uint64_t packets_processed{};
    std::uint64_t orders_processed{};
    std::uint64_t trades_executed{};
    std::uint64_t rejected_orders{};
    double throughput_packets_per_second{};
    double average_latency_us{};
    double min_latency_us{};
    double max_latency_us{};
    double p50_latency_us{};
    double p95_latency_us{};
    double p99_latency_us{};
    std::size_t queue_size{};
};

class StatisticsEngine {
public:
    StatisticsEngine();

    void record_packet();
    void record_order();
    void record_trade();
    void record_rejection();
    void record_latency(std::chrono::nanoseconds latency);
    void set_queue_size(std::size_t size);

    [[nodiscard]] StatisticsSnapshot snapshot() const;
    [[nodiscard]] std::string format_snapshot() const;

private:
    static double to_microseconds(std::chrono::nanoseconds latency);
    static double percentile(std::vector<double> values, double percentile_value);

    std::chrono::steady_clock::time_point start_time_;
    mutable std::mutex mutex_;
    std::vector<double> latency_samples_us_;
    std::uint64_t packets_processed_{0};
    std::uint64_t orders_processed_{0};
    std::uint64_t trades_executed_{0};
    std::uint64_t rejected_orders_{0};
    std::size_t queue_size_{0};
};

} // namespace cerebro
