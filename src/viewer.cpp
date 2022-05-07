#include "renderer/ModelViewerOctree.hpp"
#include "renderer/ModelViewerUV.hpp"
#include "renderer/ModelViewer.hpp"
#include "util/Config.hpp"

int main() {
    Config config("resources/config.txt");

    ModelViewer *app;
    if (config.viewerTexturePath == "UV")
        app = new ModelViewerUV(config.viewerBundlePath);
    else
        app = new ModelViewerOctree(config.viewerBundlePath, config.viewerTexturePath);

    try {
        app->run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    delete app;

    return 0;
}