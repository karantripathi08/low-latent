#pragma once

#include "cerebrox/types.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cerebrox {

class SystemSnapshot {
public:
    void ingest(const ProcessSample& sample);
    [[nodiscard]] const ProcessState* find(std::uint32_t pid) const;
    [[nodiscard]] std::vector<ProcessState> all() const;
    [[nodiscard]] std::size_t size() const;

private:
    std::vector<ProcessState> states_;
};

} // namespace cerebrox
