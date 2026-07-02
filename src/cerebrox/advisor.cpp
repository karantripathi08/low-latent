#include "cerebrox/advisor.hpp"

#include <algorithm>

namespace cerebrox {

Decision Advisor::decide(const ProcessState& state) const {
    const auto prediction = predictor_.predict(state);

    Decision decision;
    decision.pid = state.latest.pid;
    decision.process_name = state.latest.name;
    decision.classification = prediction.classification;
    decision.predicted_cpu_percent = prediction.predicted_cpu_percent;
    decision.burst_score = prediction.burst_score;
    decision.confidence = prediction.confidence;
    decision.recommendation = prediction.recommendation;
    decision.rationale = prediction.rationale;
    return decision;
}

std::vector<Decision> Advisor::rank(const std::vector<ProcessState>& states) const {
    std::vector<Decision> decisions;
    decisions.reserve(states.size());
    for (const auto& state : states) {
        decisions.push_back(decide(state));
    }

    std::sort(decisions.begin(), decisions.end(), [](const Decision& left, const Decision& right) {
        return static_cast<int>(left.recommendation) < static_cast<int>(right.recommendation);
    });
    return decisions;
}

} // namespace cerebrox
