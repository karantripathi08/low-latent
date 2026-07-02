#pragma once

#include "cerebro/async_logger.hpp"
#include "cerebro/matching_engine.hpp"
#include "cerebro/market_data_generator.hpp"
#include "cerebro/packet_decoder.hpp"
#include "cerebro/statistics_engine.hpp"

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace cerebro {

struct EngineConfig {
    std::uint32_t max_order_size{100000};
    std::uint32_t max_position_size{1000000};
    std::uint64_t packet_rate{1000};
    bool use_lock_free_queues{true};
};

class Engine {
public:
    explicit Engine(EngineConfig config = {});

    [[nodiscard]] OrderOutcome submit_order(const Order& order);
    [[nodiscard]] OrderOutcome process_packet(const std::vector<std::uint8_t>& raw_packet);
    [[nodiscard]] StatisticsSnapshot statistics() const;
    [[nodiscard]] std::string order_book_snapshot(std::size_t levels = 5) const;

    void run_simulation(std::size_t iterations);
    void run_stress_test(std::uint64_t packets_per_second, std::chrono::seconds duration);
    void replay_csv(const std::string& path);
    void benchmark(std::size_t iterations);

    [[nodiscard]] const OrderBook& order_book() const { return order_book_; }

private:
    Order to_order(const MarketPacket& packet) const;
    void print_trade(const Trade& trade) const;

    EngineConfig config_;
    PacketDecoder decoder_;
    MarketDataGenerator generator_;
    OrderBook order_book_;
    RiskEngine risk_engine_;
    MatchingEngine matching_engine_;
    StatisticsEngine statistics_;
    AsyncLogger logger_;
};

} // namespace cerebro
