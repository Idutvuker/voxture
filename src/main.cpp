#include "renderer/ModelViewerOctree.hpp"
#include "renderer/ModelViewerUV.hpp"
#include "renderer/ModelViewer.hpp"
#include "util/Config.hpp"
#include "builder/CompactOctreeBuilder.hpp"
#include "builder/FullTreeBuilder.hpp"

std::chrono::steady_clock::time_point stamp = std::chrono::steady_clock::now();

float tick() {
    using fsec = std::chrono::duration<float>;

    std::chrono::steady_clock::time_point newStamp = std::chrono::steady_clock::now();

    fsec elapsed = newStamp - stamp;

    stamp = newStamp;

    return elapsed.count();
}

void viewer(bool benchmark) {
    Config config("resources/config.txt");

    ModelViewer *app;
    if (config.viewerTexturePath == "UV")
        app = new ModelViewerUV(config.viewerBundlePath);
    else
        app = new ModelViewerOctree(config.viewerBundlePath, config.viewerTexturePath);

    if (benchmark)
        app->runBenchmark();
    else
        app->run();

    delete app;
}

void builder() {
    Config config("resources/config.txt");
//
//    FullTreeBuilder builder(config.builderBundlePath, config.builderOutPath);
//
//    tick();
//    fs::path output = builder.buildFull();

    tick();
    DiskTree::merge("data/citywall/0_282.tree", "data/citywall/282_564.tree", "data/citywall/0_564.tree");
    float elapsed = tick();

    std::cout << "Raw build finished! Elapsed time: " << elapsed << std::endl;

    CompactOctreeBuilder::build("data/citywall/0_564.tree", true).saveToDisk(config.builderOutPath + "compact");
    elapsed = tick();

    std::cout << "Compact build finished! Elapsed time: " << elapsed << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc == 1)
        throw std::invalid_argument("Not enough arguments");

    auto command = std::string(argv[1]);

    if (command == "benchmark")
        viewer(true);
    else if (command == "viewer")
        viewer(false);
    else if (command == "builder")
        builder();

    return 0;
}