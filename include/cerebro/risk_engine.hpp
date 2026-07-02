#pragma once

#include "cerebro/types.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace cerebro {

class RiskEngine {
public:
    RiskEngine(std::uint32_t max_order_size, std::uint32_t max_position_size);

    [[nodiscard]] bool validate(const Order& order, std::string& reason) const;
    void register_order(const Order& order);
    void on_trade(const Trade& trade);

private:
    std::uint32_t max_order_size_;
    std::uint32_t max_position_size_;
    std::unordered_set<std::uint64_t> seen_order_ids_;
    std::unordered_map<std::string, std::int64_t> positions_;
};

} // namespace cerebro
