#pragma once

#include "../geom/voxelizer.h"

#include <unordered_set>

namespace RendererRaylib {
    int render(const std::vector<Triangle> &triangles,
               const Voxelizer::OctreeLevels &treeLevels);
}