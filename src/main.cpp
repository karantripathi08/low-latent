#include "cerebro/cli.hpp"

int main(int argc, char** argv) {
    cerebro::Engine engine{};
    cerebro::Cli cli{engine};
    return cli.run(argc, argv);
}
