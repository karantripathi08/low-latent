#include "cerebro/market_data_generator.hpp"

#include <algorithm>
#include <chrono>

namespace cerebro {

namespace {

constexpr std::array<std::string_view, 6> kSymbols = {"AAPL", "MSFT", "GOOG", "TSLA", "NVDA", "AMZN"};

} // namespace

MarketDataGenerator::MarketDataGenerator(std::uint64_t seed)
    : rng_(seed),
      symbol_dist_(0, kSymbols.size() - 1),
      price_dist_(50.0, 500.0),
      qty_dist_(1, 1000),
      side_dist_(0, 1) {}

MarketPacket MarketDataGenerator::next_packet() {
    MarketPacket packet{};
    packet.timestamp_ns = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    const auto symbol = kSymbols[symbol_dist_(rng_)];
    std::fill(packet.symbol.begin(), packet.symbol.end(), '\0');
    std::copy_n(symbol.begin(), std::min(symbol.size(), packet.symbol.size() - 1), packet.symbol.begin());
    packet.price_ticks_value = to_ticks(price_dist_(rng_));
    packet.quantity = qty_dist_(rng_);
    packet.side = side_dist_(rng_) == 0 ? Side::Buy : Side::Sell;
    packet.order_id = next_order_id_++;
    return packet;
}

std::vector<std::uint8_t> MarketDataGenerator::next_encoded_packet(const PacketDecoder& decoder) {
    return decoder.encode(next_packet());
}

std::vector<MarketPacket> MarketDataGenerator::generate_batch(std::size_t count) {
    std::vector<MarketPacket> packets;
    packets.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        packets.push_back(next_packet());
    }
    return packets;
}

std::chrono::nanoseconds MarketDataGenerator::rate_to_period(std::uint64_t packets_per_second) {
    if (packets_per_second == 0) {
        return std::chrono::nanoseconds::max();
    }
    return std::chrono::nanoseconds{1'000'000'000LL / static_cast<std::int64_t>(packets_per_second)};
}

} // namespace cerebro
