#include "renderer/Renderer.hpp"
#include "geom/voxelizer.hpp"
#include "bundle/Bundle.hpp"

int main2() {
    Bundle saharov("resources/saharov/saharov.obj", "resources/saharov/cameras.out", "resources/saharov/list.txt");

    Voxelizer::Octree octree(saharov.mesh);

    {
        Renderer r(saharov, octree);
        r.mainLoop();
    }

    return 0;
}

#include "outofcore/App.hpp"

int main() {
    App app;
    app.run();

    return 0;
}