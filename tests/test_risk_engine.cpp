#include "cerebro/risk_engine.hpp"

#include <gtest/gtest.h>

TEST(RiskEngineTest, RejectsInvalidOrders) {
    cerebro::RiskEngine risk_engine{100, 1000};
    std::string reason;

    EXPECT_FALSE(risk_engine.validate(cerebro::Order{1, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(150.0), 0, 1}, reason));
    EXPECT_FALSE(reason.empty());

    EXPECT_FALSE(risk_engine.validate(cerebro::Order{2, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(-1.0), 10, 1}, reason));
    EXPECT_FALSE(reason.empty());

    EXPECT_FALSE(risk_engine.validate(cerebro::Order{3, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(150.0), 101, 1}, reason));
    EXPECT_FALSE(reason.empty());
}

TEST(RiskEngineTest, RejectsDuplicateOrderIds) {
    cerebro::RiskEngine risk_engine{100, 1000};
    std::string reason;
    const auto order = cerebro::Order{10, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(150.0), 10, 1};

    EXPECT_TRUE(risk_engine.validate(order, reason));
    risk_engine.register_order(order);
    EXPECT_FALSE(risk_engine.validate(order, reason));
    EXPECT_EQ(reason, "duplicate order id");
}
