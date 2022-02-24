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

#include "outofcore/TreeBuilder.hpp"

void buildOctree(const std::string &modelName) {
    Bundle bundle {"resources/models/"+modelName+".obj", "resources/testBundle/cameras.out", "resources/testBundle/list.txt"};
    Octree octree = TreeBuilder::fullVoxelization(bundle.mesh, 8);
    DiskTree::save(octree, "out3/"+modelName+".tree");
}

int main() {

//    DiskKeys::Merger::merge("out/teddy.keys", "out/cow.keys", "out/join.keys");
//    DiskKeys::Converter::convert("out/join.keys", "out/join.tree");

    DiskTree::merge("out3/teddy.tree", "out3/cow.tree", "out3/join.tree");

//    buildOctree("teddy");

    App app;
    app.run();

    return 0;
}