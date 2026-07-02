#include "cerebro/order_book.hpp"

#include <algorithm>
#include <iterator>

namespace cerebro {

std::uint32_t OrderBook::level_quantity(const PriceLevel& level) {
    std::uint32_t total = 0;
    for (const auto& order : level) {
        total += order.quantity;
    }
    return total;
}

std::optional<Trade> OrderBook::match_single(Order& incoming, Order& resting) {
    if (incoming.quantity == 0 || resting.quantity == 0) {
        return std::nullopt;
    }
    const auto quantity = std::min(incoming.quantity, resting.quantity);
    Trade trade{};
    trade.symbol = incoming.symbol;
    trade.price = resting.price;
    trade.quantity = quantity;
    trade.timestamp_ns = incoming.timestamp_ns;
    if (incoming.side == Side::Buy) {
        trade.buyer_order_id = incoming.order_id;
        trade.seller_order_id = resting.order_id;
    } else {
        trade.buyer_order_id = resting.order_id;
        trade.seller_order_id = incoming.order_id;
    }
    incoming.quantity -= quantity;
    resting.quantity -= quantity;
    return trade;
}

bool OrderBook::add_order(const Order& order) {
    if (order.side == Side::Buy) {
        auto [level_it, inserted] = bids_.try_emplace(order.price);
        auto& level = level_it->second;
        level.push_back(order);
        order_index_.emplace(order.order_id, OrderLocation{order.side, order.price, std::prev(level.end())});
        return inserted || !level.empty();
    }

    auto [level_it, inserted] = asks_.try_emplace(order.price);
    auto& level = level_it->second;
    level.push_back(order);
    order_index_.emplace(order.order_id, OrderLocation{order.side, order.price, std::prev(level.end())});
    return inserted || !level.empty();
}

bool OrderBook::cancel_order(std::uint64_t order_id) {
    const auto location_it = order_index_.find(order_id);
    if (location_it == order_index_.end()) {
        return false;
    }

    const auto location = location_it->second;
    if (location.side == Side::Buy) {
        auto level_it = bids_.find(location.price);
        if (level_it == bids_.end()) {
            return false;
        }
        level_it->second.erase(location.iterator);
        if (level_it->second.empty()) {
            bids_.erase(level_it);
        }
    } else {
        auto level_it = asks_.find(location.price);
        if (level_it == asks_.end()) {
            return false;
        }
        level_it->second.erase(location.iterator);
        if (level_it->second.empty()) {
            asks_.erase(level_it);
        }
    }

    order_index_.erase(location_it);
    return true;
}

bool OrderBook::modify_order(std::uint64_t order_id, price_ticks new_price, std::uint32_t new_quantity) {
    const auto location_it = order_index_.find(order_id);
    if (location_it == order_index_.end()) {
        return false;
    }

    const auto side = location_it->second.side;
    const auto symbol = location_it->second.iterator->symbol;
    const auto timestamp = location_it->second.iterator->timestamp_ns;
    cancel_order(order_id);
    return add_order(Order{order_id, symbol, side, new_price, new_quantity, timestamp});
}

std::optional<price_ticks> OrderBook::best_bid() const {
    if (bids_.empty()) {
        return std::nullopt;
    }
    return bids_.begin()->first;
}

std::optional<price_ticks> OrderBook::best_ask() const {
    if (asks_.empty()) {
        return std::nullopt;
    }
    return asks_.begin()->first;
}

std::vector<BookLevel> OrderBook::depth(Side side, std::size_t levels) const {
    std::vector<BookLevel> result;
    result.reserve(levels);

    if (side == Side::Buy) {
        for (const auto& [price, level] : bids_) {
            result.push_back(BookLevel{price, level_quantity(level)});
            if (result.size() == levels) {
                break;
            }
        }
    } else {
        for (const auto& [price, level] : asks_) {
            result.push_back(BookLevel{price, level_quantity(level)});
            if (result.size() == levels) {
                break;
            }
        }
    }

    return result;
}

std::vector<Trade> OrderBook::match(Order& incoming) {
    std::vector<Trade> trades;

    auto match_against = [&](auto& book, auto price_condition) {
        while (incoming.quantity > 0 && !book.empty()) {
            auto level_it = book.begin();
            if (!price_condition(level_it->first)) {
                break;
            }

            auto& level = level_it->second;
            while (incoming.quantity > 0 && !level.empty()) {
                auto resting_it = level.begin();
                if (auto trade = match_single(incoming, *resting_it)) {
                    trades.push_back(*trade);
                }
                if (resting_it->quantity == 0) {
                    order_index_.erase(resting_it->order_id);
                    level.erase(resting_it);
                }
            }

            if (level.empty()) {
                book.erase(level_it);
            }
        }
    };

    if (incoming.side == Side::Buy) {
        match_against(asks_, [&](price_ticks ask_price) { return ask_price <= incoming.price; });
    } else {
        match_against(bids_, [&](price_ticks bid_price) { return bid_price >= incoming.price; });
    }

    if (incoming.quantity > 0) {
        add_order(incoming);
    }

    return trades;
}

} // namespace cerebro
