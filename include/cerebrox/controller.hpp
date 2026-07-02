#pragma once

#include "cerebrox/advisor.hpp"
#include "cerebrox/collector.hpp"
#include "cerebrox/system_snapshot.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace cerebrox {

class Controller {
public:
    Controller() = default;

    void demo();
    void live_watch(std::size_t rounds, std::chrono::milliseconds pause);
    void benchmark(std::size_t iterations);

private:
    std::vector<Decision> analyze_trace(const std::vector<ProcessSample>& trace);
    void render_dashboard(const std::vector<Decision>& decisions) const;
    void print_dashboard_header(const std::string& title) const;
    void print_decision(const Decision& decision) const;

    Collector collector_;
    SystemSnapshot snapshot_;
    Advisor advisor_;
};

} // namespace cerebrox
