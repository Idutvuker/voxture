#include "outofcore/DiskTreeBuilder.hpp"

int main() {
    DiskTreeBuilder builder("resources/citywall/", "out/");
    builder.buildAllTrees();
    builder.mergeAll();

    return 0;
}