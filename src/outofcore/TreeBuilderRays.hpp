#pragma once

#include "DBH.hpp"

struct TreeBuilderRays {
    const std::vector<Triangle> &mesh;
    const DBH &dbh;
    explicit TreeBuilderRays(const std::vector<Triangle> &_mesh, const DBH &_dbh): mesh(_mesh), dbh(_dbh) {}

    uint maxLevel = 7;

    std::vector<glm::uvec3> voxels;
    std::vector<glm::vec3> points;

    void buildTree(const Camera &camera, float) {
        using namespace glm;

        voxels.clear();
        points.clear();

        mat4 invViewProj = inverse(camera.getViewProj());

        vec4 s = vec4(0, 0, -1, 1);
        vec4 p = invViewProj * s;
        p /= p.w;

        const auto &img = dbh.data.front();

        for (int y = 0; y < img.height; y += 5) {
            for (int x = 0; x < img.width; x += 5) {
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

//                float pointSize = Camera::linearizeDepthNDC(depthNDC) / focalLength;
//
//                uint level = 0;
//                float levelVoxelSize = 1;
//                while (level < maxLevel) {
//                    if (levelVoxelSize <= pointSize)
//                        break;
//
//                    level += 1;
//                    levelVoxelSize /= 2;
//                }

                uint level = maxLevel;
                uvec3 voxel = point * float(1 << level);
                voxels.push_back(voxel);

//                Log.info({x, y, Camera::linearizeDepthNDC(depthNDC), point, pointSize});
            }
        }
    }
};