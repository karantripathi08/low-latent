#include "cerebro/statistics_engine.hpp"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace cerebro {

StatisticsEngine::StatisticsEngine() : start_time_(std::chrono::steady_clock::now()) {}

void StatisticsEngine::record_packet() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++packets_processed_;
}

void StatisticsEngine::record_order() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++orders_processed_;
}

void StatisticsEngine::record_trade() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++trades_executed_;
}

void StatisticsEngine::record_rejection() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++rejected_orders_;
}

void StatisticsEngine::record_latency(std::chrono::nanoseconds latency) {
    std::lock_guard<std::mutex> lock(mutex_);
    latency_samples_us_.push_back(to_microseconds(latency));
    if (latency_samples_us_.size() > 100000) {
        latency_samples_us_.erase(latency_samples_us_.begin());
    }
}

void StatisticsEngine::set_queue_size(std::size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_size_ = size;
}

double StatisticsEngine::to_microseconds(std::chrono::nanoseconds latency) {
    return static_cast<double>(latency.count()) / 1000.0;
}

double StatisticsEngine::percentile(std::vector<double> values, double percentile_value) {
    if (values.empty()) {
        return 0.0;
    }
    std::sort(values.begin(), values.end());
    const auto index = static_cast<std::size_t>(std::clamp(percentile_value, 0.0, 1.0) * static_cast<double>(values.size() - 1));
    return values[index];
}

StatisticsSnapshot StatisticsEngine::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    StatisticsSnapshot snapshot{};
    snapshot.packets_processed = packets_processed_;
    snapshot.orders_processed = orders_processed_;
    snapshot.trades_executed = trades_executed_;
    snapshot.rejected_orders = rejected_orders_;
    snapshot.queue_size = queue_size_;
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time_).count();
    snapshot.throughput_packets_per_second = elapsed > 0.0 ? static_cast<double>(packets_processed_) / elapsed : 0.0;
    if (!latency_samples_us_.empty()) {
        const auto [min_it, max_it] = std::minmax_element(latency_samples_us_.begin(), latency_samples_us_.end());
        const auto sum = std::accumulate(latency_samples_us_.begin(), latency_samples_us_.end(), 0.0);
        snapshot.average_latency_us = sum / static_cast<double>(latency_samples_us_.size());
        snapshot.min_latency_us = *min_it;
        snapshot.max_latency_us = *max_it;
        snapshot.p50_latency_us = percentile(latency_samples_us_, 0.50);
        snapshot.p95_latency_us = percentile(latency_samples_us_, 0.95);
        snapshot.p99_latency_us = percentile(latency_samples_us_, 0.99);
    }
    return snapshot;
}

std::string StatisticsEngine::format_snapshot() const {
    const auto snapshot = this->snapshot();
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << "Packets processed: " << snapshot.packets_processed << '\n';
    out << "Orders processed: " << snapshot.orders_processed << '\n';
    out << "Trades executed: " << snapshot.trades_executed << '\n';
    out << "Rejected orders: " << snapshot.rejected_orders << '\n';
    out << "Throughput: " << snapshot.throughput_packets_per_second << " packets/sec\n";
    out << "Average latency: " << snapshot.average_latency_us << " us\n";
    out << "Latency min/max: " << snapshot.min_latency_us << " / " << snapshot.max_latency_us << " us\n";
    out << "p50/p95/p99: " << snapshot.p50_latency_us << " / " << snapshot.p95_latency_us << " / " << snapshot.p99_latency_us << " us\n";
    out << "Queue size: " << snapshot.queue_size << '\n';
    return out.str();
}

} // namespace cerebro
