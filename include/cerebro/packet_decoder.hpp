#pragma once

#include "cerebro/types.hpp"

#include <cstdint>
#include <vector>

namespace cerebro {

class PacketDecoder {
public:
    [[nodiscard]] std::vector<std::uint8_t> encode(const MarketPacket& packet) const;
    [[nodiscard]] bool decode(const std::vector<std::uint8_t>& raw, MarketPacket& packet) const;
};

} // namespace cerebro
