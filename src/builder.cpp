#include "outofcore/DiskTreeBuilder.hpp"

#include "util/Config.hpp"

#include "outofcore/CompactTree.hpp"

std::chrono::steady_clock::time_point stamp = std::chrono::steady_clock::now();

float tick() {
    using fsec = std::chrono::duration<float>;

    std::chrono::steady_clock::time_point newStamp = std::chrono::steady_clock::now();

    fsec elapsed = newStamp - stamp;

    stamp = newStamp;

    return elapsed.count();
}

int main() {
    Config config("resources/config.txt");

    DiskTreeBuilder builder(config.builderBundlePath, config.builderOutPath);

    tick();
    fs::path output = builder.buildAll();
    float elapsed = tick();

    std::cout << "Build finished! Elapsed: " << elapsed << std::endl;

    CompactTree tree(output.string(), false);
    tree.saveToDisk("compact/output.ctree");

    return 0;
}