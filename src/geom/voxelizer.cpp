#include "voxelizer.hpp"
#include "../util/Logger.hpp"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Voxelizer {
    bool schwarz_seidel_test(glm::vec3 voxelPos, float gridSize, const Triangle &tri) {
        using namespace glm;

        vec3 v[3], e[3], norm;
        vec3 p, n;

        v[0] = tri[0] * gridSize - voxelPos;
        v[1] = tri[1] * gridSize - voxelPos;
        v[2] = tri[2] * gridSize - voxelPos;

        e[0] = v[1] - v[0];
        e[1] = v[2] - v[1];
        e[2] = v[0] - v[2];

        norm = normalize(cross(e[0], e[1]));

        float r = 0.5f*fabsf(norm.x) + 0.5f*fabsf(norm.y) + 0.5f*fabsf(norm.z);
        float s = norm.x*(0.5f - v[0].x) + norm.y*(0.5f - v[0].y) + norm.z*(0.5f - v[0].z);
        if (fabsf(s) > r) return false;

        vec3 c((norm.x >= 0) ? 1 : -1, (norm.y >= 0) ? 1 : -1, (norm.z >= 0) ? 1 : -1);
        if (-(-e[0].y*v[0].x + e[0].x*v[0].y)*c.z + fmaxf(0, -e[0].y*c.z) + fmaxf(0, e[0].x*c.z) < 0) return false;
        if (-(-e[1].y*v[1].x + e[1].x*v[1].y)*c.z + fmaxf(0, -e[1].y*c.z) + fmaxf(0, e[1].x*c.z) < 0) return false;
        if (-(-e[2].y*v[2].x + e[2].x*v[2].y)*c.z + fmaxf(0, -e[2].y*c.z) + fmaxf(0, e[2].x*c.z) < 0) return false;
        if (-(-e[0].x*v[0].z + e[0].z*v[0].x)*c.y + fmaxf(0, -e[0].x*c.y) + fmaxf(0, e[0].z*c.y) < 0) return false;
        if (-(-e[1].x*v[1].z + e[1].z*v[1].x)*c.y + fmaxf(0, -e[1].x*c.y) + fmaxf(0, e[1].z*c.y) < 0) return false;
        if (-(-e[2].x*v[2].z + e[2].z*v[2].x)*c.y + fmaxf(0, -e[2].x*c.y) + fmaxf(0, e[2].z*c.y) < 0) return false;
        if (-(-e[0].z*v[0].y + e[0].y*v[0].z)*c.x + fmaxf(0, -e[0].z*c.x) + fmaxf(0, e[0].y*c.x) < 0) return false;
        if (-(-e[1].z*v[1].y + e[1].y*v[1].z)*c.x + fmaxf(0, -e[1].z*c.x) + fmaxf(0, e[1].y*c.x) < 0) return false;
        if (-(-e[2].z*v[2].y + e[2].y*v[2].z)*c.x + fmaxf(0, -e[2].z*c.x) + fmaxf(0, e[2].y*c.x) < 0) return false;

        return true;
    }

    void voxelizeTriangle(const Triangle &tri, VoxelSet &voxelSet) {
        const auto grid_size = float(voxelSet.getGridSize());

        using namespace glm;

        auto aabbMin = tri[0];
        auto aabbMax = tri[0];

        for (int i = 1; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                aabbMin[j] = std::min(aabbMin[j], tri[i][j]);
                aabbMax[j] = std::max(aabbMax[j], tri[i][j]);
            }
        }

        ivec3 start = aabbMin * grid_size;
        ivec3 end = ceil(aabbMax * grid_size);

        for (int z = start.z; z < end.z; z++) {
            for (int y = start.y; y < end.y; y++) {
                for (int x = start.x; x < end.x; x++) {
                    vec3 p{x, y, z};

                    if (schwarz_seidel_test(p, grid_size, tri))
                        voxelSet.insert(Voxel{p});
                }
            }
        }
    }

    VoxelSet voxelize(const std::vector<Triangle> &triangles) {
        VoxelSet voxelSet(VOXELIZE_LEVEL);

        for (const auto &tri: triangles)
            voxelizeTriangle(tri, voxelSet);

        return voxelSet;
    }

    bool Voxel::operator==(const Voxel &other) const {
        return pos == other.pos;
    }

    size_t VoxelHash::operator()(const Voxel &voxel) const {
        return ((voxel.pos.x << 20) + (voxel.pos.y << 10) + voxel.pos.z);
    }
}