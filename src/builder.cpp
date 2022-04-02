#include "outofcore/DiskTreeBuilder.hpp"

#include "util/Config.hpp"

//int main() {
//    Config config("resources/config.txt");
//
//    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
//    builder.buildAllTrees();
//    builder.mergeAll();
//
//    return 0;
//}

#include "outofcore/CompactTree.hpp"

int main() {
    Config config("resources/config.txt");

    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
    fs::path output = builder.buildAll();

    Octree octree(output.string());
    CompactTree compactTree(octree);
    compactTree.saveToDisk(config.builderOutPath + "final.ctree");

    return 0;
}