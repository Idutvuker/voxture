#pragma once

#include "DBH.hpp"
#include <unordered_set>

struct TreeBuilderRays {
    const std::vector<Triangle> &mesh;
    const DBH &dbh;
    explicit TreeBuilderRays(const std::vector<Triangle> &_mesh, const DBH &_dbh): mesh(_mesh), dbh(_dbh) {}

    uint maxLevel = 10;

//    struct Voxel {
//        glm::uvec3 pos;
//        uint level;
//        bool operator==(const Voxel &other) const = default;
//    };
//
//    struct VoxelHash {
//        size_t operator()(const Voxel &voxel) const {
//            return (size_t(voxel.level) << 60) & (size_t(voxel.pos.x) << 32) & (size_t(voxel.pos.y) << 16) & (size_t(voxel.pos.z));
//        }
//    };
//
//    std::unordered_set<Voxel, VoxelHash> voxelSet;

    /// Stores child addresses instead of offsets
    /// Not ordered
    Octree tmpOctree;

    Octree octree;

    std::vector<glm::vec3> points;

    void buildTree(const Camera &camera, float focalLength) {
        using namespace glm;

//        voxelSet.clear();
        points.clear();

        tmpOctree.data.clear();
        tmpOctree.data.emplace_back();

        mat4 invViewProj = inverse(camera.getViewProj());

        vec4 s = vec4(0, 0, -1, 1);
        vec4 p = invViewProj * s;
        p /= p.w;

        const auto &img = dbh.data.front();

        for (int y = 0; y < img.height; y += 1) {
            for (int x = 0; x < img.width; x += 1) {
                float depthNDC = img.get({x, y}) * 2 - 1;

                if (depthNDC == 1)
                    continue;

                vec2 texCoord = (vec2(x, y) + 0.5f) / vec2(img.width, img.height) * 2.f - 1.f;
                vec4 S = vec4(texCoord.x, texCoord.y, depthNDC, 1);
                vec4 P = invViewProj * S;

                vec3 point(P / P.w);
                if (!all(lessThan(point, vec3(1))))
                    continue;

                points.push_back(point);

                const uvec3 minVox = point * float(1 << maxLevel);

                uvec3 curVox = {0, 0, 0};
                uint level = 0;

                const float pointSize = Camera::linearizeDepthNDC(depthNDC) / focalLength;
                float levelVoxelSize = 1;

                uint curPtr = 0;

                while (level < maxLevel) {
                    if (levelVoxelSize <= pointSize)
                        break;

                    level += 1;
                    levelVoxelSize /= 2;

                    uint levelDiff = maxLevel - level;
                    uvec3 nextVox = minVox >> levelDiff;

                    uvec3 diff = nextVox - curVox * uint(2);
                    uint child = voxelOffsetToIndex(diff);

                    uint32_t childPtr = tmpOctree.data[curPtr].children[child];

                    if (childPtr == 0) {
                        childPtr = tmpOctree.data.size();
                        tmpOctree.data.emplace_back();

                        tmpOctree.data[curPtr].children[child] = childPtr;
                    }

                    curVox = nextVox;
                    curPtr = childPtr;
                }


//                uint level = maxLevel;
//                uvec3 voxel = point * float(1 << level);

//                voxelSet.insert({voxel, level});
//
//                while (level != 0) {
//                    level--;
//                    voxel /= 2;
//                    voxelSet.insert({voxel, level});
//                }

//                voxels.push_back(voxel);

//                Log.info({x, y, Camera::linearizeDepthNDC(depthNDC), point, pointSize});
            }
        }

        octree.data.clear();

        dfs(0);

//        Log.info({"Building octree"});
//
//        octree.data.clear();
//
//        dfs(0, {0, 0, 0});
    }

    uint32_t dfs(uint32_t v) {
        uint32_t id = octree.data.size();
        octree.data.emplace_back();

        for (uint i = 0; i < 8; i++) {
            auto childAddr = tmpOctree.data[v].children[i];
            if (childAddr != 0)
                octree.data[id].children[i] = dfs(childAddr) - id;
        }

        return id;
    }

    /*uint32_t dfs(uint level, glm::uvec3 pos) {
        Voxel vox{pos, level};

        if (!voxelSet.contains(vox))
            return 0;

        octree.data.emplace_back();
        auto id = octree.data.size() - 1;

        for (uint i = 0; i < VOX_OFFSET.size(); i++) {
            auto childAddr = dfs(level + 1, pos * uint(2) + VOX_OFFSET[i]);
            octree.data[id].children[i] = childAddr == 0 ? 0 : childAddr - id;
        }

        return id;
    }*/
};