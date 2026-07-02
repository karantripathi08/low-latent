#include "cerebrox/collector.hpp"

#include <chrono>
#include <fstream>
#include <sstream>

namespace cerebrox {

ProcessSample Collector::make_sample(std::uint32_t pid,
                                     std::string name,
                                     double cpu_percent,
                                     double memory_mb,
                                     std::uint32_t threads,
                                     std::uint64_t context_switches,
                                     std::uint64_t io_bytes_per_sec,
                                     bool foreground) {
    return ProcessSample{pid,
                         std::move(name),
                         cpu_percent,
                         memory_mb,
                         threads,
                         context_switches,
                         io_bytes_per_sec,
                         foreground,
                         std::chrono::steady_clock::now()};
}

std::vector<ProcessSample> Collector::synthetic_trace() const {
    return {
        make_sample(101, "chrome", 18.0, 1450.0, 32, 1200, 250'000, false),
        make_sample(202, "valorant", 24.0, 2200.0, 18, 2400, 1'500'000, true),
        make_sample(303, "zoom", 14.0, 900.0, 14, 1600, 180'000, true),
        make_sample(404, "docker", 42.0, 2600.0, 40, 9000, 2'100'000, false),
        make_sample(505, "database", 55.0, 4100.0, 72, 12000, 5'000'000, false),
    };
}

std::vector<ProcessSample> Collector::sample_once() const {
#if defined(__linux__)
    auto samples = sample_linux_procfs();
    if (!samples.empty()) {
        return samples;
    }
#endif
    return synthetic_trace();
}

#if defined(__linux__)
std::vector<ProcessSample> Collector::sample_linux_procfs() const {
    std::vector<ProcessSample> samples;
    const auto synthetic = synthetic_trace();
    samples.insert(samples.end(), synthetic.begin(), synthetic.end());
    return samples;
}
#endif

} // namespace cerebrox
