#include "outofcore/ModelViewer.hpp"

#include "util/Config.hpp"

#include "renderer/SimpleRenderer.hpp"

int main() {
//    try {
//        Config config("resources/config.txt");
//
//        ModelViewer app(config.viewerBundlePath, config.viewerTreePath);
//        app.run();
//    } catch (const std::exception &e) {
//        std::cerr << e.what() << std::endl;
//    }


    try {
        Config config("resources/config.txt");

        SimpleRenderer app(config.viewerBundlePath);
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
//    std::vector<Triangle> mesh;
//
//    getTexTriangles("resources/citywall/model.ply", mesh);

    return 0;
}