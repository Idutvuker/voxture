#include "outofcore/DiskTreeBuilder.hpp"

#include "util/Config.hpp"

int main() {
    Config config("resources/config.txt");

    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
    builder.buildAllTrees();
    builder.mergeAll();

    return 0;
}