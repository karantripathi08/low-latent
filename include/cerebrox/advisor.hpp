#pragma once

#include "cerebrox/predictor.hpp"

#include <vector>

namespace cerebrox {

class Advisor {
public:
    [[nodiscard]] Decision decide(const ProcessState& state) const;
    [[nodiscard]] std::vector<Decision> rank(const std::vector<ProcessState>& states) const;

private:
    Predictor predictor_;
};

} // namespace cerebrox
