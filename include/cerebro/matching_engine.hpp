#pragma once

#include "cerebro/order_book.hpp"
#include "cerebro/risk_engine.hpp"

namespace cerebro {

class MatchingEngine {
public:
    MatchingEngine(RiskEngine& risk_engine, OrderBook& order_book);

    [[nodiscard]] OrderOutcome submit(Order order);

private:
    RiskEngine& risk_engine_;
    OrderBook& order_book_;
};

} // namespace cerebro
