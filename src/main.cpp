#include "renderer/renderer.hpp"
#include "util/Status.hpp"
#include "geom/triangle.hpp"
#include "geom/voxelizer.hpp"
#include "util/saharov.hpp"
#include "geom/normalizer.hpp"

#include <iostream>

#include <glm/vec3.hpp>

int main() {
    SaharovLoader saharov;
    saharov.load();

    Normalizer norm(saharov.triangles);


    Voxelizer::VoxelSet voxelSet = Voxelizer::voxelize(saharov.triangles);
    auto colors = Voxelizer::colorize(voxelSet);
    auto treeLevels = Voxelizer::buildLevels(voxelSet, colors);
    treeLevels.buildRaw();


//    {
//        Renderer::RenderData data{treeLevels, saharov.triangles, norm.center};
//        Renderer r(data);
//        r.mainLoop();
//    }

    return 0;
}
