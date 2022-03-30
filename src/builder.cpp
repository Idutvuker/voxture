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
//    Config config("resources/config.txt");
//
//    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
//    builder.buildAll();


    Octree octree("out/0_32.tree");
    std::cout << "Octree loaded " << octree.data.size() << std::endl;

    CompactTree compactTree(octree);
    std::cout << compactTree.data.size() << std::endl;
    compactTree.saveToDisk("compact/saharov.ctree");

    return 0;
}