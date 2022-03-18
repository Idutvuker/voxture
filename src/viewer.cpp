#include "geom/voxelizer.hpp"
#include "bundle/Bundle.hpp"

#include "outofcore/ModelViewer.hpp"
#include "outofcore/Octree.hpp"

#include "outofcore/TreeBuilder.hpp"

void buildOctree(const std::string &modelName) {
    Bundle bundle {"resources/models/"+modelName+".obj", "resources/testBundle/cameras.out", "resources/testBundle/list.txt"};
    Octree octree = TreeBuilder::fullVoxelization(bundle.mesh, 8);
    DiskTree::save(octree, "out3/"+modelName+".tree");
}

std::chrono::steady_clock::time_point stamp = std::chrono::steady_clock::now();

float tick() {
    using fsec =  std::chrono::duration<float>;

    std::chrono::steady_clock::time_point newStamp = std::chrono::steady_clock::now();

    fsec elapsed = newStamp - stamp;

    stamp = newStamp;

    return elapsed.count();
}


void compareTimes() {
    std::string first = "0";
    std::string second = "13";

//    tick();
//    DiskKeys::Merger::merge("out5/"+first+".keys", "out5/"+second+".keys", "out5/test.keys");
//
//    float elapsed1 = tick();
//    printf("Elapsed keys merge: %.3f sec\n", elapsed1);

    tick();
    DiskTree::merge("out4/"+first+".tree", "out4/"+second+".tree", "out4/test.tree");

    float elapsed2 = tick();
    printf("Elapsed tree merge: %.3f sec\n", elapsed2);
}

int main() {
    ModelViewer app;
    app.run();

    return 0;
}