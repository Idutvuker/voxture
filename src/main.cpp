#include "renderer/renderer.hpp"
#include "geom/voxelizer.hpp"
#include "bundle/Bundle.hpp"

int main() {
    Bundle saharov("resources/saharov/saharov.obj", "resources/saharov/cameras.out", "resources/saharov/list.txt");

    Voxelizer::Octree octree(saharov.mesh);

    {
        Renderer r(saharov, octree);
        r.mainLoop();
    }

    return 0;
}
