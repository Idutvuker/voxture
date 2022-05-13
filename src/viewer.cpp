#include "renderer/ModelViewerOctree.hpp"
#include "renderer/ModelViewerUV.hpp"
#include "renderer/ModelViewer.hpp"
#include "util/Config.hpp"

int main(int argc, char *argv[]) {
    Config config("resources/config.txt");

    ModelViewer *app;
    if (config.viewerTexturePath == "UV")
        app = new ModelViewerUV(config.viewerBundlePath);
    else
        app = new ModelViewerOctree(config.viewerBundlePath, config.viewerTexturePath);

    try {
        if (argc > 1 && std::string(argv[1]) == "benchmark")
            app->runBenchmark();
        else
            app->run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    delete app;

    return 0;
}