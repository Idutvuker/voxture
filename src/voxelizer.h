#pragma once

#include "triangle.h"

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <glm/vec3.hpp>

namespace Voxelizer {
    using u32 = uint32_t;
    using u64 = uint64_t;

    const u32 GRID_SIZE = 16;
    const float SPACE = 10;

    struct Voxel {
        glm::uvec3 pos;
        bool operator==(const Voxel &other) const;
    };

    struct VoxelHash {
        size_t operator () (const Voxel &voxel) const;
    };

    std::vector<Voxel> voxelizeTriangle(const Triangle &tri);
    void voxelize(const std::vector<Triangle> &triangles);
}