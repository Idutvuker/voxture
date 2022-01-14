#pragma once

#include "Triangle.hpp"

#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <queue>

namespace Voxelizer {
    using u32 = uint32_t;
    using u64 = uint64_t;

    const u32 VOXELIZE_LEVEL = 7;

    struct Voxel {
        glm::ivec3 pos;
        bool operator==(const Voxel &other) const;
    };

    struct VoxelHash {
        size_t operator () (const Voxel &voxel) const;
    };

    struct VoxelSet {
        std::unordered_set<Voxel, VoxelHash> set;

        u32 level;

        u32 getGridSize() const {
            return 1 << level;
        }

        void insert(const Voxel &voxel) {
            const glm::ivec3 bounds = glm::ivec3(int(getGridSize()));

            if (all(glm::lessThan(voxel.pos, bounds))) {
                set.insert(voxel);
            }
        }

        VoxelSet() : level(0) {}
        explicit VoxelSet(u32 level) : level(level) {}
    };

    using VoxelColors = std::unordered_map<Voxel, glm::u8vec3, VoxelHash>;

    bool schwarz_seidel_test(glm::vec3 voxelPos, float gridSize, const Triangle &tri);

    void voxelizeTriangle(const Triangle &tri, VoxelSet &voxelSet);
    VoxelSet voxelize(const std::vector<Triangle> &triangles);

    inline VoxelColors colorize(const VoxelSet& voxelSet) {
        using namespace glm;

        VoxelColors res;

        const auto gridSize = float(voxelSet.getGridSize());

        for (const auto &v: voxelSet.set)
            res[v] = u8vec3(vec3(v.pos) / (gridSize - 1) * 255.f);

        return res;
    }

    struct Octree {
        std::vector<VoxelSet> levels;
        VoxelColors colors; // ColorMap of the last level

        struct Node {
            static constexpr u32 WHITE = 0x00ffffff;
            static constexpr u32 ADDR_BIT = 1 << 31;

            u32 vox[8];
        };

        std::vector<Node> rawData;

        explicit Octree(const std::vector<Triangle> &mesh) {
            VoxelSet voxelSet = voxelize(mesh);

//            colors = colorize(voxelSet);

            buildLevels(voxelSet);
            buildRaw();
        }

        void buildLevels(VoxelSet &origSet) {
            const auto depth = origSet.level;

            levels.resize(depth + 1);

            levels[depth] = std::move(origSet);

            for (u32 i = 0; i < depth; i++) {
                VoxelSet &newSet = levels[depth - i - 1];
                newSet.level = (depth - i - 1);

                for (const auto &v: levels[depth - i].set) {
                    newSet.insert(Voxel{.pos = v.pos / 2});
                }
            }
        }

        static constexpr std::array<glm::ivec3, 8> VOX_OFFSET = {
                glm::ivec3(0, 0, 0),
                glm::ivec3(0, 0, 1),
                glm::ivec3(0, 1, 0),
                glm::ivec3(0, 1, 1),

                glm::ivec3(1, 0, 0),
                glm::ivec3(1, 0, 1),
                glm::ivec3(1, 1, 0),
                glm::ivec3(1, 1, 1),
        };

        struct Temp {
            size_t depth;
            Voxel voxel;
        };

        void buildRaw() {
            rawData.clear();

            std::queue<Temp> q;

            q.push(Temp{0, { .pos = {0, 0, 0} }});
            u32 treeSize = 1;

            while(!q.empty()) {
                Temp t = q.front();
                q.pop();

                auto &level = levels.at(t.depth);

                Node node{};

                auto it = level.set.find(t.voxel);
                const bool curInSet = it != level.set.end();

                for (size_t i = 0; i < VOX_OFFSET.size(); i++) {
                    Voxel newVoxel = { .pos = t.voxel.pos * 2 + VOX_OFFSET[i] };

                    if (curInSet) {
                        if (t.depth == levels.size() - 2) {
                            glm::uvec3 colorVec = colors[newVoxel];
                            u32 color = (colorVec.r << 16) + (colorVec.g << 8) + (colorVec.b);

                            node.vox[i] = color;
                        } else {
                            q.push(Temp{t.depth + 1, newVoxel});

                            u32 index = Node::ADDR_BIT | treeSize;
                            node.vox[i] = index;
                            treeSize++;
                        }
                    } else {
                        node.vox[i] = Node::WHITE;
                    }
                }

                rawData.push_back(node);
            }
        }

    };

}