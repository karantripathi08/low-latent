#include "cerebro/packet_decoder.hpp"

#include <gtest/gtest.h>

TEST(PacketDecoderTest, RoundTripEncodesAndDecodesPacket) {
    cerebro::PacketDecoder decoder;
    cerebro::MarketPacket packet{};
    packet.timestamp_ns = 123456;
    packet.symbol = {'A', 'A', 'P', 'L', '\0', '\0', '\0', '\0'};
    packet.price_ticks_value = cerebro::to_ticks(150.20);
    packet.quantity = 100;
    packet.side = cerebro::Side::Buy;
    packet.order_id = 42;

    const auto encoded = decoder.encode(packet);
    cerebro::MarketPacket decoded{};

    ASSERT_TRUE(decoder.decode(encoded, decoded));
    EXPECT_EQ(decoded.timestamp_ns, packet.timestamp_ns);
    EXPECT_EQ(decoded.symbol, packet.symbol);
    EXPECT_EQ(decoded.price_ticks_value, packet.price_ticks_value);
    EXPECT_EQ(decoded.quantity, packet.quantity);
    EXPECT_EQ(decoded.side, packet.side);
    EXPECT_EQ(decoded.order_id, packet.order_id);
}
