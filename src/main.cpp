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

void viewer(const Config &config, bool benchmark) {
    ModelViewer *app;
    if (config.viewerTexturePath == "UV")
        app = new ModelViewerUV(config.bundlePath);
    else
        app = new ModelViewerOctree(config.bundlePath, config.viewerTexturePath);

    if (benchmark)
        app->runBenchmark();
    else
        app->run();

    delete app;
}

void builder(const Config &config) {
    FullTreeBuilder builder(config.bundlePath, config.builderOutPath);

    tick();
    fs::path output = builder.buildFull();

    float elapsed = tick();

    std::cout << "Raw build finished! Elapsed time: " << elapsed << std::endl;

    CompactOctreeBuilder::build(output, true).saveToDisk(config.builderOutPath / "compact");
    elapsed = tick();

    std::cout << "Compact build finished! Elapsed time: " << elapsed << std::endl;
}

int main(int argc, char *argv[]) {
    Config config("resources/config.txt");

    if (argc == 1)
        throw std::invalid_argument("Not enough arguments");

    auto command = std::string(argv[1]);

    if (command == "benchmark")
        viewer(config, true);
    else if (command == "viewer")
        viewer(config, false);
    else if (command == "builder")
        builder(config);

    return 0;
}