#pragma once

#include "../geom/intersect.hpp"

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

    uint maxLevel = 0;
    const std::vector<Triangle> &mesh;
    std::vector <Voxelizer::Voxel> voxels;

    explicit TreeBuilder(const std::vector<Triangle> &_mesh): mesh(_mesh) {}

    void buildTree(Camera camera) {
        voxels.clear();

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

    bool dfs(uint level, glm::uvec3 vox, const TriIndices &relevant) {
        if (level > maxLevel)
            return false;

        const uint gridSize = 1 << level;
        const float voxelSize = 1.f / float(gridSize);

        TriIndices intersection;

        for (const auto &triId: relevant) {
            if (intersect::triCubeOverlap(glm::vec3(vox) * voxelSize, voxelSize, mesh[triId])) {
                intersection.push_back(triId);
            }
        }

        if (intersection.empty())
            return false;

        bool isLeaf = true;
        for (const auto &offs: VOX_OFFSET)
            if (dfs(level + 1, vox * uint(2) + offs, intersection))
                isLeaf = false;

        if (isLeaf)
            voxels.push_back(Voxelizer::Voxel{vox});

        return true;
    }

};