#include "cerebrox/predictor.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace cerebrox {

double Predictor::moving_average_cpu(const ProcessState& state) {
    if (state.history.empty()) {
        return 0.0;
    }

    double total = 0.0;
    for (const auto& sample : state.history) {
        total += sample.cpu_percent;
    }
    return total / static_cast<double>(state.history.size());
}

double Predictor::cpu_slope(const ProcessState& state) {
    if (state.history.size() < 2) {
        return 0.0;
    }
    return state.history.back().cpu_percent - state.history.front().cpu_percent;
}

double Predictor::memory_growth(const ProcessState& state) {
    if (state.history.size() < 2) {
        return 0.0;
    }
    return state.history.back().memory_mb - state.history.front().memory_mb;
}

ProcessClass Predictor::classify(const ProcessState& state) {
    const auto& sample = state.latest;
    if (sample.foreground && (sample.name.find("game") != std::string::npos || sample.name.find("valorant") != std::string::npos)) {
        return ProcessClass::Game;
    }
    if (sample.foreground && (sample.name.find("meet") != std::string::npos || sample.name.find("zoom") != std::string::npos)) {
        return ProcessClass::Interactive;
    }
    if (sample.io_bytes_per_sec > 1'000'000 || sample.context_switches > 10'000) {
        return ProcessClass::Server;
    }
    if (sample.cpu_percent > 40.0) {
        return ProcessClass::Background;
    }
    return ProcessClass::Unknown;
}

Recommendation Predictor::recommend(const ProcessState& state, const WorkloadPrediction& prediction) {
    const auto& sample = state.latest;
    if (prediction.classification == ProcessClass::Game || prediction.classification == ProcessClass::Interactive) {
        return Recommendation::ProtectForeground;
    }
    if (prediction.burst_score > 0.7 && prediction.predicted_cpu_percent > 70.0) {
        return Recommendation::PinToCore;
    }
    if (sample.io_bytes_per_sec > 1'000'000) {
        return Recommendation::BatchIOTasks;
    }
    if (sample.foreground && sample.cpu_percent > 60.0) {
        return Recommendation::IncreasePriority;
    }
    if (!sample.foreground && sample.cpu_percent > 30.0) {
        return Recommendation::DecreasePriority;
    }
    if (!sample.foreground) {
        return Recommendation::MoveToBackground;
    }
    return Recommendation::NoChange;
}

WorkloadPrediction Predictor::predict(const ProcessState& state) const {
    WorkloadPrediction prediction;
    prediction.classification = classify(state);

    const auto avg_cpu = moving_average_cpu(state);
    const auto slope = cpu_slope(state);
    const auto mem_growth = memory_growth(state);

    prediction.predicted_cpu_percent = std::clamp(avg_cpu + slope * 0.75, 0.0, 100.0);
    prediction.burst_score = std::clamp((std::abs(slope) / 50.0) + (mem_growth > 0.0 ? std::min(mem_growth / 1024.0, 0.5) : 0.0), 0.0, 1.0);
    prediction.confidence = std::clamp(0.45 + (state.history.size() * 0.1), 0.0, 0.95);
    prediction.recommendation = recommend(state, prediction);

    if (prediction.classification == ProcessClass::Game) {
        prediction.rationale = "foreground game with rising resource demand";
    } else if (prediction.classification == ProcessClass::Interactive) {
        prediction.rationale = "latency-sensitive foreground workload";
    } else if (prediction.classification == ProcessClass::Server) {
        prediction.rationale = "server-like workload with high I/O or context switching";
    } else if (prediction.burst_score > 0.7) {
        prediction.rationale = "bursty workload with accelerating CPU demand";
    } else {
        prediction.rationale = "stable workload with low scheduling pressure";
    }

    return prediction;
}

} // namespace cerebrox
