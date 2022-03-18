#include "outofcore/DiskTreeBuilder.hpp"

int main() {
    DiskTreeBuilder builder;
    builder.buildAllTrees();
    builder.mergeAll();

    return 0;
}