#include "outofcore/ModelViewer.hpp"

#include "util/Config.hpp"

int main() {
    Config config("resources/config.txt");

    ModelViewer app(config.viewerBundlePath, config.viewerTreePath);
    app.run();

    return 0;
}