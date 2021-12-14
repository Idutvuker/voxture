#include "renderer/renderer.hpp"
#include "geom/voxelizer.hpp"
#include "bundle/Bundle.hpp"

#include <glm/vec3.hpp>

int main() {
    Bundle saharov;
    saharov.load();

    //Normalizer norm(saharov.triangles);


//    Voxelizer::VoxelSet voxelSet = Voxelizer::voxelize(saharov.triangles);
//    auto colors = Voxelizer::colorize(voxelSet);
//    auto treeLevels = Voxelizer::buildLevels(voxelSet, colors);
//    treeLevels.buildRaw();

    auto octree = Voxelizer::Octree({}, {});
    {
        Renderer::RenderData data{saharov, octree, glm::vec3(0)};
        Renderer r(data);
        r.mainLoop();
    }

    return 0;
}
