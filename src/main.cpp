#include "cerebrox/cli.hpp"

int main(int argc, char** argv) {
    cerebrox::Cli cli{};
    return cli.run(argc, argv);
}
