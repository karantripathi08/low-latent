#pragma once

#include "cerebro/packet_decoder.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <random>
#include <vector>

namespace cerebro {

class MarketDataGenerator {
public:
    explicit MarketDataGenerator(std::uint64_t seed = std::random_device{}());

    [[nodiscard]] MarketPacket next_packet();
    [[nodiscard]] std::vector<std::uint8_t> next_encoded_packet(const PacketDecoder& decoder);
    [[nodiscard]] std::vector<MarketPacket> generate_batch(std::size_t count);
    [[nodiscard]] static std::chrono::nanoseconds rate_to_period(std::uint64_t packets_per_second);

private:
    std::mt19937_64 rng_;
    std::uniform_int_distribution<std::size_t> symbol_dist_;
    std::uniform_real_distribution<double> price_dist_;
    std::uniform_int_distribution<std::uint32_t> qty_dist_;
    std::uniform_int_distribution<int> side_dist_;
    std::uint64_t next_order_id_{100};
};

} // namespace cerebro
