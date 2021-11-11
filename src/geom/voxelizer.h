#pragma once

#include "triangle.h"

#include <vector>
#include <unordered_set>
#include <cstdint>
#include <glm/vec3.hpp>

namespace Voxelizer {
    using u32 = uint32_t;
    using u64 = uint64_t;

    const u32 VOXELIZE_LEVEL = 6;

    struct Voxel {
        glm::ivec3 pos;
        bool operator==(const Voxel &other) const;
    };

    struct VoxelHash {
        size_t operator () (const Voxel &voxel) const;
    };

    struct VoxelSet {
        std::unordered_set<Voxel, VoxelHash> set;

        const u32 level;

        u32 getGridSize() const {
            return 1 << level;
        }

        void insert(const Voxel &voxel) {
            set.insert(voxel);
        }

        explicit VoxelSet(u32 level) : level(level) {}
    };

    struct OctreeLevels {
        std::vector<VoxelSet> data;
    };

    void voxelizeTriangle(const Triangle &tri, VoxelSet &voxelSet);
    VoxelSet voxelize(const std::vector<Triangle> &triangles);

    inline OctreeLevels buildLevels(const VoxelSet &origSet) {
        const auto levels = origSet.level;

        OctreeLevels res;
        res.data.reserve(levels + 1);

        res.data.push_back(origSet);
        for (u32 i = 1; i <= origSet.level; i++) {
            VoxelSet newSet(levels - i);

            for (const auto &v: res.data[i - 1].set) {
                newSet.insert(Voxel{.pos = v.pos / 2});
            }

            res.data.push_back(std::move(newSet));
        }

        return res;
    }
}