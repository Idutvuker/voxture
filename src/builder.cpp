#include "outofcore/DiskTreeBuilder.hpp"

int main() {
    DiskTreeBuilder builder("resources/saharov/", "out/");
    builder.buildAllTrees();
    builder.mergeAll();

    return 0;
}