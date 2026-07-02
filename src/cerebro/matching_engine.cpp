#include "cerebro/matching_engine.hpp"

#include <string>
#include <utility>

namespace cerebro {

MatchingEngine::MatchingEngine(RiskEngine& risk_engine, OrderBook& order_book)
    : risk_engine_(risk_engine), order_book_(order_book) {}

OrderOutcome MatchingEngine::submit(Order order) {
    OrderOutcome outcome{};
    std::string reason;
    if (!risk_engine_.validate(order, reason)) {
        outcome.accepted = false;
        outcome.rejection_reason = std::move(reason);
        return outcome;
    }

    risk_engine_.register_order(order);
    outcome.trades = order_book_.match(order);
    outcome.accepted = true;
    for (const auto& trade : outcome.trades) {
        risk_engine_.on_trade(trade);
    }
    return outcome;
}

} // namespace cerebro
