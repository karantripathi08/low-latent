#include "cerebro/engine.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace cerebro {

namespace {

std::string symbol_from_packet(const MarketPacket& packet) {
    return std::string(packet.symbol.data());
}

} // namespace

Engine::Engine(EngineConfig config)
    : config_(config),
      generator_(),
      risk_engine_(config.max_order_size, config.max_position_size),
      matching_engine_(risk_engine_, order_book_) {
    logger_.log(LogLevel::Info, "Cerebro engine initialized");
}

Order Engine::to_order(const MarketPacket& packet) const {
    return Order{packet.order_id, symbol_from_packet(packet), packet.side, packet.price_ticks_value, packet.quantity, packet.timestamp_ns};
}

OrderOutcome Engine::submit_order(const Order& order) {
    statistics_.record_order();
    const auto start = std::chrono::high_resolution_clock::now();
    auto outcome = matching_engine_.submit(order);
    const auto end = std::chrono::high_resolution_clock::now();
    statistics_.record_latency(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start));
    if (outcome.accepted) {
        for (const auto& trade : outcome.trades) {
            statistics_.record_trade();
            print_trade(trade);
        }
    } else {
        statistics_.record_rejection();
        logger_.log(LogLevel::Warning, "Order rejected: " + outcome.rejection_reason);
    }
    statistics_.set_queue_size(order_book_.resting_orders());
    return outcome;
}

OrderOutcome Engine::process_packet(const std::vector<std::uint8_t>& raw_packet) {
    statistics_.record_packet();
    MarketPacket packet{};
    const auto decode_start = std::chrono::high_resolution_clock::now();
    if (!decoder_.decode(raw_packet, packet)) {
        statistics_.record_rejection();
        return OrderOutcome{false, "decode failure", {}};
    }
    const auto decode_end = std::chrono::high_resolution_clock::now();
    statistics_.record_latency(std::chrono::duration_cast<std::chrono::nanoseconds>(decode_end - decode_start));
    return submit_order(to_order(packet));
}

StatisticsSnapshot Engine::statistics() const {
    return statistics_.snapshot();
}

std::string Engine::order_book_snapshot(std::size_t levels) const {
    std::ostringstream out;
    out << "\n## ASKS\n";
    for (const auto& level : order_book_.depth(Side::Sell, levels)) {
        out << std::fixed << std::setprecision(2) << to_price(level.price) << " | " << level.quantity << '\n';
    }
    out << "\n## BIDS\n";
    for (const auto& level : order_book_.depth(Side::Buy, levels)) {
        out << std::fixed << std::setprecision(2) << to_price(level.price) << " | " << level.quantity << '\n';
    }
    return out.str();
}

void Engine::run_simulation(std::size_t iterations) {
    for (std::size_t index = 0; index < iterations; ++index) {
        const auto packet = generator_.next_packet();
        const auto encoded = decoder_.encode(packet);
        process_packet(encoded);
    }
}

void Engine::run_stress_test(std::uint64_t packets_per_second, std::chrono::seconds duration) {
    const auto period = MarketDataGenerator::rate_to_period(packets_per_second);
    const auto start = std::chrono::steady_clock::now();
    std::size_t sent = 0;
    while (std::chrono::steady_clock::now() - start < duration) {
        const auto packet = generator_.next_packet();
        const auto encoded = decoder_.encode(packet);
        process_packet(encoded);
        ++sent;
        if (period != std::chrono::nanoseconds::max()) {
            std::this_thread::sleep_for(period);
        }
    }
    logger_.log(LogLevel::Info, "Stress test completed; packets sent: " + std::to_string(sent));
}

void Engine::replay_csv(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to open CSV: " + path);
    }

    std::string line;
    std::getline(input, line);
    while (std::getline(input, line)) {
        std::stringstream stream(line);
        std::string timestamp;
        std::string symbol;
        std::string side;
        std::string price;
        std::string quantity;
        std::getline(stream, timestamp, ',');
        std::getline(stream, symbol, ',');
        std::getline(stream, side, ',');
        std::getline(stream, price, ',');
        std::getline(stream, quantity, ',');
        const auto order = Order{
            static_cast<std::uint64_t>(std::stoull(timestamp)),
            symbol,
            side == "BUY" ? Side::Buy : Side::Sell,
            to_ticks(std::stod(price)),
            static_cast<std::uint32_t>(std::stoul(quantity)),
            static_cast<std::uint64_t>(std::stoull(timestamp))
        };
        submit_order(order);
    }
}

void Engine::benchmark(std::size_t iterations) {
    for (std::size_t index = 0; index < iterations; ++index) {
        const auto packet = generator_.next_packet();
        const auto encoded = decoder_.encode(packet);
        const auto start = std::chrono::high_resolution_clock::now();
        process_packet(encoded);
        const auto end = std::chrono::high_resolution_clock::now();
        statistics_.record_latency(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start));
    }
    logger_.log(LogLevel::Info, "Benchmark completed with " + std::to_string(iterations) + " iterations");
}

void Engine::print_trade(const Trade& trade) const {
    std::cout << "TRADE EXECUTED\n";
    std::cout << "BUYER : " << trade.buyer_order_id << '\n';
    std::cout << "SELLER : " << trade.seller_order_id << '\n';
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "PRICE : " << to_price(trade.price) << '\n';
    std::cout << "QTY : " << trade.quantity << '\n';
}

} // namespace cerebro
