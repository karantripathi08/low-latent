#include "cerebro/packet_decoder.hpp"

#include <cstring>

namespace cerebro {

namespace {

template <typename T>
void append_bytes(std::vector<std::uint8_t>& buffer, const T& value) {
    const auto* data = reinterpret_cast<const std::uint8_t*>(&value);
    buffer.insert(buffer.end(), data, data + sizeof(T));
}

template <typename T>
bool read_bytes(const std::vector<std::uint8_t>& buffer, std::size_t& offset, T& value) {
    if (offset + sizeof(T) > buffer.size()) {
        return false;
    }
    std::memcpy(&value, buffer.data() + offset, sizeof(T));
    offset += sizeof(T);
    return true;
}

} // namespace

std::vector<std::uint8_t> PacketDecoder::encode(const MarketPacket& packet) const {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(8 + 8 + 8 + 4 + 1 + 8);
    append_bytes(buffer, packet.timestamp_ns);
    append_bytes(buffer, packet.symbol);
    append_bytes(buffer, packet.price_ticks_value);
    append_bytes(buffer, packet.quantity);
    append_bytes(buffer, packet.side);
    append_bytes(buffer, packet.order_id);
    return buffer;
}

bool PacketDecoder::decode(const std::vector<std::uint8_t>& raw, MarketPacket& packet) const {
    std::size_t offset = 0;
    if (!read_bytes(raw, offset, packet.timestamp_ns)) {
        return false;
    }
    if (!read_bytes(raw, offset, packet.symbol)) {
        return false;
    }
    if (!read_bytes(raw, offset, packet.price_ticks_value)) {
        return false;
    }
    if (!read_bytes(raw, offset, packet.quantity)) {
        return false;
    }
    if (!read_bytes(raw, offset, packet.side)) {
        return false;
    }
    if (!read_bytes(raw, offset, packet.order_id)) {
        return false;
    }
    return true;
}

} // namespace cerebro
