#pragma once

#include "cerebrox/controller.hpp"

namespace cerebrox {

class Cli {
public:
    Cli() = default;

    int run(int argc, char** argv);

private:
    int run_demo();
    int run_live();
    int run_benchmark();

    Controller controller_;
};

} // namespace cerebrox
