#pragma once

#include "../geom/intersect.hpp"
#include "../geom/Frustum.hpp"
#include "DBH.hpp"

#include "Octree.hpp"
#include "../common/constants.hpp"
#include "Timer.hpp"

struct TreeBuilder {
    using TriIndices = std::vector<size_t>;

    const std::vector<Triangle> &mesh;
    const DBH &dbh;
    explicit TreeBuilder(const std::vector<Triangle> &_mesh, const DBH &_dbh): mesh(_mesh), dbh(_dbh) {}

    uint maxLevel = 0;

    Frustum frustum;
    glm::mat4 MVPMat;

    Octree octree;

    Timer timer;
    glm::vec2 imgDims;

    void buildTree(const Camera &camera, const Image<glm::u8vec3> &img) {
        frustumTime = 0;
        depthTime = 0;
        intersectTime = 0;

        imgDims = glm::vec2(img.width, img.height);

        octree.data.clear();

        MVPMat = camera.getViewProj();
        frustum = Frustum(MVPMat);

        TriIndices allIndices(mesh.size());
        for (size_t i = 0; i < mesh.size(); i++)
            allIndices[i] = i;

        dfs(0, glm::uvec3(0), allIndices);

        printf("frust %f\n", frustumTime);
        printf("depth %f\n", depthTime);
        printf("intersect %f\n", intersectTime);

        /*for (size_t i = 0; i < octree.data.size(); i++) {
            const auto &node = octree.data[i];
            printf("%zu %d: ", i, node.isLeaf());
            for (uint j = 0; j < 8; j++)
                printf("%u ", node.children[j]);
            printf("\n");
        }*/

//        timer.tick();
//        for (const auto &vox: voxels) {
//            glm::vec3 pos = (glm::vec3(vox.pos) + 0.5f) / float(1 << maxLevel);
//            glm::vec4 S = MVPMat * glm::vec4(pos, 1.0);
//            S /= S.w;
//
//            glm::vec2 texCoord = (glm::vec2(S.x, -S.y) + 1.f) / 2.f;
//
//            colors.push_back(img.getByTexCoord(texCoord));
//        }
//
//        double colorTime = timer.tick();
//        printf("color %f\n", colorTime);
    }


    double frustumTime;
    double depthTime;
    double intersectTime;

    // TODO: Optimize first level
    uint32_t dfs(uint level, const glm::uvec3 &vox, const TriIndices &relevant) {
        using namespace glm;

        assert (level <= maxLevel);

        const float voxelSize = 1.f / float(1 << level);
        vec3 pos = vec3(vox) * voxelSize;

        bool isLeaf = level == maxLevel;

//        ResolutionCheck: {
//            vec4 S = MVPMat * vec4(pos, 1.0);
//            vec2 texCoord = (vec2(S.x, S.y) / S.w + 1.f) / 2.f;
//
//            uvec2 pixel1 = texCoord * imgDims;
//
//            S = MVPMat * vec4(pos + voxelSize, 1.0);
//            texCoord = (vec2(S.x, S.y) / S.w + 1.f) / 2.f;
//
//            uvec2 pixel2 = texCoord * imgDims;
//
//            if (pixel1 == pixel2)
//                return false;
//        }

        timer.tick();

        FrustumTest: {
            if (!frustum.IsBoxVisible(pos, pos + vec3(voxelSize)))
                return 0;
        }
        frustumTime += timer.tick();

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
        intersectTime += timer.tick();

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

            float modelMaxDepth = dbh.queryMaxApprox(aabbMin, aabbMax) * 2 - 1;

            // Resolution test
            if (modelMaxDepth < 0) {
                isLeaf = true;
            }

            if (aabbMinDepth >= modelMaxDepth) {
//                Log.info({vox, aabbMinDepth, modelMaxDepth, aabbMin, aabbMax});
                return 0;
            }
        }

        depthTime += timer.tick();

        octree.data.emplace_back();
        size_t id = octree.data.size() - 1;

        if (!isLeaf) {
            for (uint i = 0; i < VOX_OFFSET.size(); i++) {
                octree.data[id].children[i] = dfs(level + 1, vox * uint(2) + VOX_OFFSET[i], intersection);
            }
        }

        return id;
    }

};