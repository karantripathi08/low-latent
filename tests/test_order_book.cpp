#include "cerebro/order_book.hpp"

#include <gtest/gtest.h>

TEST(OrderBookTest, MaintainsBestBidAndAsk) {
    cerebro::OrderBook book;
    book.add_order(cerebro::Order{1, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(150.00), 500, 1});
    book.add_order(cerebro::Order{2, "AAPL", cerebro::Side::Buy, cerebro::to_ticks(149.90), 350, 2});
    book.add_order(cerebro::Order{3, "AAPL", cerebro::Side::Sell, cerebro::to_ticks(150.30), 100, 3});
    book.add_order(cerebro::Order{4, "AAPL", cerebro::Side::Sell, cerebro::to_ticks(150.10), 400, 4});

    ASSERT_TRUE(book.best_bid().has_value());
    ASSERT_TRUE(book.best_ask().has_value());
    EXPECT_EQ(book.best_bid().value(), cerebro::to_ticks(150.00));
    EXPECT_EQ(book.best_ask().value(), cerebro::to_ticks(150.10));
}

TEST(OrderBookTest, SupportsCancelAndModify) {
    cerebro::OrderBook book;
    book.add_order(cerebro::Order{10, "MSFT", cerebro::Side::Sell, cerebro::to_ticks(320.00), 50, 1});

    EXPECT_TRUE(book.modify_order(10, cerebro::to_ticks(319.50), 25));
    ASSERT_TRUE(book.best_ask().has_value());
    EXPECT_EQ(book.best_ask().value(), cerebro::to_ticks(319.50));
    EXPECT_TRUE(book.cancel_order(10));
    EXPECT_FALSE(book.best_ask().has_value());
}
