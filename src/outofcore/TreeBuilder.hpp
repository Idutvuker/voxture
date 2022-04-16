#pragma once

#include "../geom/intersect.hpp"
#include "../geom/Frustum.hpp"
#include "DBH.hpp"

#include "Octree.hpp"
#include "../common/constants.hpp"

struct TreeBuilder {
    using TriIndices = std::vector<size_t>;

    const std::vector<Triangle> &mesh;
    const DBH &dbh;
    explicit TreeBuilder(const std::vector<Triangle> &_mesh, const DBH &_dbh): mesh(_mesh), dbh(_dbh) {}

    uint maxLevel = 0;

    Frustum frustum;
    glm::mat4 MVPMat;

    Octree octree{};

    glm::vec2 imgDims;

    void buildTree(const glm::mat4 &a_MVPMat, const Image<glm::u8vec3> &img) {
        MVPMat = a_MVPMat;
        frustum = Frustum(MVPMat);

        octree.data.clear();
        imgDims = glm::vec2(img.width, img.height);

        TriIndices allIndices(mesh.size());
        for (size_t i = 0; i < mesh.size(); i++)
            allIndices[i] = i;

        dfs(0, glm::uvec3(0), allIndices);
    }

    uint32_t dfs(uint level, const glm::uvec3 &vox, const TriIndices &relevant) {
        using namespace glm;

        assert (level <= maxLevel);

        const float voxelSize = 1.f / float(1 << level);
        vec3 pos = vec3(vox) * voxelSize;

        bool isLeaf = level == maxLevel;

        ResolutionCheck: {
            vec4 S = MVPMat * vec4(pos, 1.0);
            vec2 texCoord = (vec2(S.x, S.y) / S.w + 1.f) / 2.f;

            uvec2 pixel1 = texCoord * imgDims;

            S = MVPMat * vec4(pos + voxelSize, 1.0);
            texCoord = (vec2(S.x, S.y) / S.w + 1.f) / 2.f;

            uvec2 pixel2 = texCoord * imgDims;

            if (pixel1 == pixel2)
                return false;
        }


        FrustumTest: {
            if (!frustum.IsBoxVisible(pos, pos + vec3(voxelSize)))
                return 0;
        }

        TriIndices intersection;

        IntersectionTest: {
            for (const auto &triId: relevant) {
                if (intersect::triCubeOverlap(pos, voxelSize, mesh[triId])) {
                    intersection.push_back(triId);
                }
            }

            if (intersection.empty())
                return 0;
        }

        DepthTest: {
            float aabbMinDepth = 1.0f;

            // TexCoord AABB
            vec2 aabbMin(1);
            vec2 aabbMax(0);

            for (const auto &offs: VOX_OFFSET) {
                vec3 P = vec3(vox + offs) * voxelSize;
                vec4 S = MVPMat * vec4(P, 1.0);
                S /= S.w;

                vec2 texCoord = (vec2(S.x, S.y) + 1.f) / 2.f;

                aabbMinDepth = min(aabbMinDepth, S.z);

                aabbMin = min(aabbMin, texCoord);
                aabbMax = max(aabbMax, texCoord);
            }

            aabbMin = max(aabbMin, vec2(0));
            aabbMax = min(aabbMax, vec2(1));

            float modelMaxDepth = dbh.queryMax(aabbMin, aabbMax) * 2 - 1;

            // Resolution test
            if (modelMaxDepth < 0) {
                isLeaf = true;
            }

            if (aabbMinDepth >= modelMaxDepth) {
                return 0;
            }
        }

        octree.data.emplace_back();
        size_t id = octree.data.size() - 1;

        if (!isLeaf) {
            for (uint i = 0; i < VOX_OFFSET.size(); i++) {
                uint32_t childAddr = dfs(level + 1, vox * uint(2) + VOX_OFFSET[i], intersection);
                octree.data[id].children[i] = childAddr == 0 ? 0 : childAddr - id;
            }
        }

        return id;
    }

    static Octree fullVoxelization(const std::vector<Triangle> &mesh, uint maxLevel) {
        Octree octree;

        TriIndices allIndices(mesh.size());
        for (size_t i = 0; i < mesh.size(); i++)
            allIndices[i] = i;

        dfsFull(0, glm::uvec3(0), allIndices, mesh, octree, maxLevel);

        return octree;
    }

    static uint32_t dfsFull(uint level, const glm::uvec3 &vox, const TriIndices &relevant, const std::vector<Triangle> &mesh, Octree &octree, uint maxLevel) {
        using namespace glm;

        assert (level <= maxLevel);

        const float voxelSize = 1.f / float(1 << level);
        vec3 pos = vec3(vox) * voxelSize;

        bool isLeaf = level == maxLevel;

        TriIndices intersection;

        IntersectionTest: {
            for (const auto &triId: relevant) {
                if (intersect::triCubeOverlap(pos, voxelSize, mesh[triId])) {
                    intersection.push_back(triId);
                }
            }

            if (intersection.empty())
                return 0;
        }

        octree.data.emplace_back();
        size_t id = octree.data.size() - 1;

        if (!isLeaf) {
            for (uint i = 0; i < VOX_OFFSET.size(); i++) {
                uint32_t childAddr = dfsFull(level + 1, vox * uint(2) + VOX_OFFSET[i], intersection, mesh, octree, maxLevel);
                octree.data[id].children[i] = childAddr == 0 ? 0 : childAddr - id;
            }
        }

        return id;
    }

};