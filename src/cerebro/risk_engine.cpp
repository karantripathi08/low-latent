#include "cerebro/risk_engine.hpp"

#include <cstdlib>
#include <cmath>

namespace cerebro {

RiskEngine::RiskEngine(std::uint32_t max_order_size, std::uint32_t max_position_size)
    : max_order_size_(max_order_size), max_position_size_(max_position_size) {}

bool RiskEngine::validate(const Order& order, std::string& reason) const {
    if (order.quantity == 0) {
        reason = "invalid quantity";
        return false;
    }
    if (order.quantity > max_order_size_) {
        reason = "max order size exceeded";
        return false;
    }
    if (order.price < 0) {
        reason = "negative price";
        return false;
    }
    if (seen_order_ids_.contains(order.order_id)) {
        reason = "duplicate order id";
        return false;
    }

    const auto current_position = positions_.contains(order.symbol) ? positions_.at(order.symbol) : 0;
    const auto signed_quantity = order.side == Side::Buy ? static_cast<std::int64_t>(order.quantity)
                                                         : -static_cast<std::int64_t>(order.quantity);
    const auto projected_position = current_position + signed_quantity;
    if (std::llabs(projected_position) > static_cast<std::int64_t>(max_position_size_)) {
        reason = "max position size exceeded";
        return false;
    }
    return true;
}

void RiskEngine::register_order(const Order& order) {
    seen_order_ids_.insert(order.order_id);
    const auto signed_quantity = order.side == Side::Buy ? static_cast<std::int64_t>(order.quantity)
                                                         : -static_cast<std::int64_t>(order.quantity);
    positions_[order.symbol] += signed_quantity;
}

void RiskEngine::on_trade(const Trade&) {
}

} // namespace cerebro
