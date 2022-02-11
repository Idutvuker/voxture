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
#include "outofcore/Octree.hpp"

int main() {
    App app;
    app.run();

    Keys2Tree::convert("out/teddy.keys", "out/teddy.tree");

//    uint64_t x = 160842843834661;
//    printf("%zu\n", (x << 3) >> 3);


    //    Merger merger();

    return 0;
}