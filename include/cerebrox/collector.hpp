#pragma once

#include "cerebrox/types.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace cerebrox {

class Collector {
public:
    [[nodiscard]] std::vector<ProcessSample> sample_once() const;
    [[nodiscard]] std::vector<ProcessSample> synthetic_trace() const;

private:
    static ProcessSample make_sample(std::uint32_t pid,
                                     std::string name,
                                     double cpu_percent,
                                     double memory_mb,
                                     std::uint32_t threads,
                                     std::uint64_t context_switches,
                                     std::uint64_t io_bytes_per_sec,
                                     bool foreground);

#if defined(__linux__)
    [[nodiscard]] std::vector<ProcessSample> sample_linux_procfs() const;
#endif
};

} // namespace cerebrox
