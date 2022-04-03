#include "outofcore/DiskTreeBuilder.hpp"

#include "util/Config.hpp"

#include "outofcore/CompactTree.hpp"

int main() {
//    Config config("resources/config.txt");
//
//    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
//    fs::path output = builder.buildAll();

    CompactTree tree("outSah/join_31.tree", false);
    tree.saveToDisk("compact/test.ctree");

    return 0;
}