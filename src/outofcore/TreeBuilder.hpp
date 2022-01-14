#pragma once

#include "../geom/intersect.hpp"
#include "../geom/Frustum.hpp"

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
    explicit TreeBuilder(const std::vector<Triangle> &_mesh): mesh(_mesh) {}

    uint maxLevel = 0;

    Frustum frustum;

    std::vector <Voxelizer::Voxel> voxels;

    void buildTree(Camera _camera) {
        voxels.clear();

        frustum = Frustum(_camera.projection * _camera.view);

        TriIndices allIndices(mesh.size());
        for (size_t i = 0; i < mesh.size(); i++)
            allIndices[i] = i;

        dfs(0, glm::uvec3(0), allIndices);

//        glm::mat4 MVPMat = camera.projection * camera.view;
//
//        const uint gridSize = 1 << maxLevel;
//        const float voxelSize = 1.f / float(gridSize);
//
//        for (uint x = 0; x < gridSize; x++) {
//            for (uint y = 0; y < gridSize; y++) {
//                for (uint z = 0; z < gridSize; z++) {
//
//                    glm::uvec3 pos(x, y, z);
//                    for (const auto &tri: mesh) {
//                        if (intersect::triCubeOverlap(glm::vec3(pos) * voxelSize, voxelSize, tri)) {
//                            voxels.push_back(Voxelizer::Voxel{pos});
//                            break;
//                        }
//                    }
//
//                }
//            }
//        }
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