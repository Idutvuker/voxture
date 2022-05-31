#include "builder/FullTreeBuilder.hpp"

#include "util/Config.hpp"

#include "data/CompactOctree.hpp"

std::chrono::steady_clock::time_point stamp = std::chrono::steady_clock::now();

float tick() {
    using fsec = std::chrono::duration<float>;

    std::chrono::steady_clock::time_point newStamp = std::chrono::steady_clock::now();

    fsec elapsed = newStamp - stamp;

    stamp = newStamp;

    return elapsed.count();
}

int main() {
//    Config config("resources/config.txt");
//
//    FullTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
//
//    tick();
//    fs::path output = builder.buildFull();
//    float elapsed = tick();
//
//    std::cout << "Build finished! Elapsed time: " << elapsed << std::endl;

//    CompactOctreeBuilder::build("out/big.tree").saveToDisk("presentation/compact");

    CompactOctree t("presentation/compact");

    t.compressColors();

    return 0;
}