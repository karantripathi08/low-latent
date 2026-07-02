#pragma once

#include "cerebro/engine.hpp"

#include <cstdint>
#include <string>

namespace cerebro {

class Cli {
public:
    explicit Cli(Engine& engine);

    int run(int argc, char** argv);

private:
    int run_simulate();
    int run_demo();
    int run_benchmark();
    int run_replay(const std::string& path);
    int run_stress(std::uint64_t rate);
    int run_orderbook();
    void interactive_menu();
    void inject_order(Side side);

    Engine& engine_;
};

} // namespace cerebro
