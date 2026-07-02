#pragma once

#include "cerebro/price.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cerebro {

enum class Side : std::uint8_t { Buy = 0, Sell = 1 };

inline std::string_view to_string(Side side) {
    return side == Side::Buy ? std::string_view{"BUY"} : std::string_view{"SELL"};
}

struct MarketPacket {
    std::uint64_t timestamp_ns{};
    std::array<char, 8> symbol{};
    price_ticks price_ticks_value{};
    std::uint32_t quantity{};
    Side side{Side::Buy};
    std::uint64_t order_id{};
};

struct Order {
    std::uint64_t order_id{};
    std::string symbol;
    Side side{Side::Buy};
    price_ticks price{};
    std::uint32_t quantity{};
    std::uint64_t timestamp_ns{};
};

struct Trade {
    std::uint64_t buyer_order_id{};
    std::uint64_t seller_order_id{};
    std::string symbol;
    price_ticks price{};
    std::uint32_t quantity{};
    std::uint64_t timestamp_ns{};
};

struct BookLevel {
    price_ticks price{};
    std::uint32_t quantity{};
};

struct OrderOutcome {
    bool accepted{false};
    std::string rejection_reason;
    std::vector<Trade> trades;
};

} // namespace cerebro
