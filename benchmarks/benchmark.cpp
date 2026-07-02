#include "cerebro/engine.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

int main() {
    cerebro::Engine engine{};
    const auto start = std::chrono::high_resolution_clock::now();
    engine.benchmark(25000);
    const auto end = std::chrono::high_resolution_clock::now();
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    const auto stats = engine.statistics();
    std::cout << "Cerebro Benchmark Report\n";
    std::cout << "Total time: " << elapsed_ms << " ms\n";
    std::cout << "Packets processed: " << stats.packets_processed << "\n";
    std::cout << "Orders processed: " << stats.orders_processed << "\n";
    std::cout << "Trades executed: " << stats.trades_executed << "\n";
    std::cout << "Rejected orders: " << stats.rejected_orders << "\n";
    std::cout << "Average latency: " << std::fixed << std::setprecision(2) << stats.average_latency_us << " us\n";
    std::cout << "p50: " << stats.p50_latency_us << " us\n";
    std::cout << "p95: " << stats.p95_latency_us << " us\n";
    std::cout << "p99: " << stats.p99_latency_us << " us\n";
    return 0;
}
