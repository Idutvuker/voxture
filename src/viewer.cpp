#include "renderer/ModelViewerOctree.hpp"

#include "util/Config.hpp"

#include "renderer/ModelViewerUV.hpp"

int main() {
    try {
        Config config("resources/config.txt");

        ModelViewerOctree app(config.viewerBundlePath, config.viewerTreePath);
//        SimpleRenderer app(config.viewerBundlePath);
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}