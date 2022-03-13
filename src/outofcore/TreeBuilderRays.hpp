#pragma once

#include "DBH.hpp"
#include <unordered_set>

struct TreeBuilderRays {
    const std::vector<Triangle> &mesh;
    explicit TreeBuilderRays(const std::vector<Triangle> &_mesh): mesh(_mesh) {}

    uint maxLevel = 14;

    std::vector<glm::vec3> points;

    Octree buildTree(const glm::mat4 &viewProjMat, float focalLength, const Image<float> &depthMap, const Image<glm::u8vec3> &photo) {
        assert(depthMap.width == photo.width && depthMap.height == photo.height);

        Octree octree;

        // Stores child addresses instead of offsets
        // Not ordered
        Octree tmpOctree;

        using namespace glm;

        points.clear();

        tmpOctree.data.clear();
        tmpOctree.data.reserve(photo.width * photo.height * 2);

        tmpOctree.data.emplace_back();

        mat4 invViewProj = inverse(viewProjMat);

        for (int y = 0; y < depthMap.height; y++) {
            for (int x = 0; x < depthMap.width; x++) {
                float depthNDC = depthMap.get({x, y}) * 2 - 1;

                if (depthNDC == 1)
                    continue;

                vec2 texCoord = (vec2(x, y) + 0.5f) / vec2(depthMap.width, depthMap.height) * 2.f - 1.f;
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

                u8vec3 colorVec = photo.get({x, photo.height - y - 1});
                u32 color = (colorVec.r << 16) | (colorVec.g << 8) | colorVec.b;

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
                        tmpOctree.data.back().color = color;

                        tmpOctree.data[curPtr].children[child] = childPtr;
                    }

                    curVox = nextVox;
                    curPtr = childPtr;
                }
            }
        }

        octree.data.clear();
        octree.data.reserve(tmpOctree.data.size());

        if (!tmpOctree.data.empty())
            dfs(0, tmpOctree, octree);

        return octree;
    }

    static uint32_t dfs(uint32_t v, const Octree &tmpOctree, Octree &octree) {
        const auto &tmpNode = tmpOctree.data[v];

        uint32_t id = octree.data.size();
        octree.data.emplace_back();
        octree.data.back().color = tmpNode.color;

        for (uint i = 0; i < 8; i++) {
            auto childAddr = tmpNode.children[i];
            if (childAddr != 0) {
                octree.data[id].children[i] = dfs(childAddr, tmpOctree, octree) - id;
            }
        }

        return id;
    }
};