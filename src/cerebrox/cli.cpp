#include "cerebrox/cli.hpp"

#include <chrono>
#include <iostream>

namespace cerebrox {

int Cli::run(int argc, char** argv) {
    if (argc < 2) {
        return run_demo();
    }

    const std::string mode = argv[1];
    if (mode == "demo") {
        return run_demo();
    }
    if (mode == "live") {
        return run_live();
    }
    if (mode == "benchmark") {
        return run_benchmark();
    }

    return run_demo();
}

int Cli::run_demo() {
    controller_.demo();
    return 0;
}

int Cli::run_live() {
    controller_.live_watch(3, std::chrono::milliseconds{600});
    return 0;
}

int Cli::run_benchmark() {
    controller_.benchmark(250);
    return 0;
}

} // namespace cerebrox
