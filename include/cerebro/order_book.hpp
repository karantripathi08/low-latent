#pragma once

#include "cerebro/types.hpp"

#include <list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cerebro {

class OrderBook {
public:
    bool add_order(const Order& order);
    bool cancel_order(std::uint64_t order_id);
    bool modify_order(std::uint64_t order_id, price_ticks new_price, std::uint32_t new_quantity);

    [[nodiscard]] std::optional<price_ticks> best_bid() const;
    [[nodiscard]] std::optional<price_ticks> best_ask() const;
    [[nodiscard]] std::vector<BookLevel> depth(Side side, std::size_t levels) const;
    [[nodiscard]] std::vector<Trade> match(Order& incoming);

    [[nodiscard]] std::size_t resting_orders() const { return order_index_.size(); }

private:
    struct OrderLocation {
        Side side{};
        price_ticks price{};
        std::list<Order>::iterator iterator;
    };

    using PriceLevel = std::list<Order>;
    using BidBook = std::map<price_ticks, PriceLevel, std::greater<price_ticks>>;
    using AskBook = std::map<price_ticks, PriceLevel>;

    BidBook bids_;
    AskBook asks_;
    std::unordered_map<std::uint64_t, OrderLocation> order_index_;

    [[nodiscard]] static std::uint32_t level_quantity(const PriceLevel& level);
    [[nodiscard]] static std::optional<Trade> match_single(Order& incoming, Order& resting);
};

} // namespace cerebro
