#pragma once

#include "cerebrox/types.hpp"

namespace cerebrox {

class Predictor {
public:
    [[nodiscard]] WorkloadPrediction predict(const ProcessState& state) const;

private:
    static double moving_average_cpu(const ProcessState& state);
    static double cpu_slope(const ProcessState& state);
    static double memory_growth(const ProcessState& state);
    static ProcessClass classify(const ProcessState& state);
    static Recommendation recommend(const ProcessState& state, const WorkloadPrediction& prediction);
};

} // namespace cerebrox
