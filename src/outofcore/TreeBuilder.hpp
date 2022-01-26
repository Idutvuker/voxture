#pragma once

#include "../geom/intersect.hpp"
#include "../geom/Frustum.hpp"
#include "DBH.hpp"

struct TreeBuilder {
    using TriIndices = std::vector<size_t>;

    static constexpr std::array<glm::uvec3, 8> VOX_OFFSET = {
            glm::uvec3(0, 0, 0),
            glm::uvec3(0, 0, 1),
            glm::uvec3(0, 1, 0),
            glm::uvec3(0, 1, 1),

            glm::uvec3(1, 0, 0),
            glm::uvec3(1, 0, 1),
            glm::uvec3(1, 1, 0),
            glm::uvec3(1, 1, 1),
    };

    const std::vector<Triangle> &mesh;
    const DBH &dbh;
    explicit TreeBuilder(const std::vector<Triangle> &_mesh, const DBH &_dbh): mesh(_mesh), dbh(_dbh) {}

    uint maxLevel = 0;

    Frustum frustum;
    glm::mat4 MVPMat;

    std::vector <Voxelizer::Voxel> voxels;
    std::vector <glm::u8vec3> colors;

    void buildTree(const Camera &camera, const Image<glm::u8vec3> &img) {
        voxels.clear();
        colors.clear();

        MVPMat = camera.getViewProj();
        frustum = Frustum(MVPMat);

        TriIndices allIndices(mesh.size());
        for (size_t i = 0; i < mesh.size(); i++)
            allIndices[i] = i;

        dfs(0, glm::uvec3(0), allIndices);

        for (const auto &vox: voxels) {
            glm::vec3 pos = glm::vec3(vox.pos) / float(1 << maxLevel);
            glm::vec4 S = MVPMat * glm::vec4(pos, 1.0);
            S /= S.w;

            glm::vec2 texCoord = (glm::vec2(S.x, -S.y) + 1.f) / 2.f;
            Log.info({vox.pos, texCoord});

            colors.push_back(img.getByTexCoord(texCoord));
        }
    }


    // TODO: Optimize first level
    bool dfs(uint level, const glm::uvec3 &vox, const TriIndices &relevant) {
        using namespace glm;

        if (level > maxLevel)
            return false;

        const float voxelSize = 1.f / float(1 << level);
        vec3 pos = vec3(vox) * voxelSize;

        TriIndices intersection;

        FrustumTest: {
            if (!frustum.IsBoxVisible(pos, pos + vec3(voxelSize)))
                return false;
        }

        DepthTest: {
            float minDepth = 1.0f;

            // TexCoord AABB
            vec2 aabbMin(1);
            vec2 aabbMax(0);

            for (const auto &offs: VOX_OFFSET) {
                vec3 P = vec3(vox + offs) * voxelSize;
                vec4 S = MVPMat * vec4(P, 1.0);
                S /= S.w;

                vec2 texCoord = (vec2(S.x, S.y) + 1.f) / 2.f;

                minDepth = min(minDepth, S.z);

                aabbMin = min(aabbMin, texCoord);
                aabbMax = max(aabbMax, texCoord);
            }

            aabbMin = max(aabbMin, vec2(0));
            aabbMax = min(aabbMax, vec2(1));

            float modelDepth = dbh.queryMax(aabbMin, aabbMax) * 2 - 1;

            if (minDepth >= modelDepth)
                return false;
        }

        IntersectionTest: {
            for (const auto &triId: relevant) {
                if (intersect::triCubeOverlap(pos, voxelSize, mesh[triId])) {
                    intersection.push_back(triId);
                }
            }

            if (intersection.empty())
                return false;
        }

        for (const auto &offs: VOX_OFFSET)
            dfs(level + 1, vox * uint(2) + offs, intersection);

        if (level == maxLevel) {
            voxels.push_back(Voxelizer::Voxel{vox});
        }

        return true;
    }

};