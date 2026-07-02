#include "cerebrox/system_snapshot.hpp"

namespace cerebrox {

void SystemSnapshot::ingest(const ProcessSample& sample) {
    for (auto& state : states_) {
        if (state.latest.pid == sample.pid) {
            state.history.push_back(sample);
            state.latest = sample;
            return;
        }
    }

    ProcessState state;
    state.latest = sample;
    state.history.push_back(sample);
    states_.push_back(std::move(state));
}

const ProcessState* SystemSnapshot::find(std::uint32_t pid) const {
    for (const auto& state : states_) {
        if (state.latest.pid == pid) {
            return &state;
        }
    }
    return nullptr;
}

std::vector<ProcessState> SystemSnapshot::all() const {
    return states_;
}

std::size_t SystemSnapshot::size() const {
    return states_.size();
}

} // namespace cerebrox
