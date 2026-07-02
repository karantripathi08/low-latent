#include "cerebro/matching_engine.hpp"

#include <gtest/gtest.h>

TEST(MatchingEngineTest, ExecutesCrossingOrders) {
    cerebro::OrderBook book;
    cerebro::RiskEngine risk_engine{1000, 10000};
    cerebro::MatchingEngine matching_engine{risk_engine, book};

    const auto sell_result = matching_engine.submit(cerebro::Order{1, "AAPL", cerebro::Side::Sell, cerebro::to_ticks(150.20), 100, 1});
    ASSERT_TRUE(sell_result.accepted);
    EXPECT_TRUE(sell_result.trades.empty());

    const auto buy_result = matching_engine.submit(cerebro::Order{2, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(150.20), 100, 2});
    ASSERT_TRUE(buy_result.accepted);
    ASSERT_EQ(buy_result.trades.size(), 1U);
    EXPECT_EQ(buy_result.trades.front().quantity, 100U);
    EXPECT_EQ(buy_result.trades.front().price, cerebro::to_ticks(150.20));
}
