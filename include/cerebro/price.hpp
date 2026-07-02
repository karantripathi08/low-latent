#pragma once

#include <cmath>
#include <cstdint>

namespace cerebro {

using price_ticks = std::int64_t;
constexpr price_ticks kPriceScale = 100;

inline price_ticks to_ticks(double price) {
    return static_cast<price_ticks>(std::llround(price * static_cast<double>(kPriceScale)));
}

inline double to_price(price_ticks ticks) {
    return static_cast<double>(ticks) / static_cast<double>(kPriceScale);
}

} // namespace cerebro
