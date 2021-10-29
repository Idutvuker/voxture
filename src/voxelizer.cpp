#include "voxelizer.h"
#include "Logger.h"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

namespace Voxelizer {
    std::vector<Voxel> voxelizeTriangle(const Triangle &tri) {
        const float EPS = 1e-5;

        using namespace glm;

        std::vector<Voxel> res;
        vec3 v[3];

        for (size_t i = 0; i < 3; i++)
            v[i] = tri[i] * float(GRID_SIZE - 1);

        vec3 anorm = abs(normalize(cross(v[1] - v[0], v[2] - v[0])));

        int nAxis = 0;
        for (int i = 1; i < 3; i++)
            if (anorm[i] > anorm[nAxis])
                nAxis = i;

        printf("nAxis = %d\n", nAxis);

        int ra = nAxis == 1 ? 0 : 1; // raster axis
        int rb = nAxis == 2 ? 0 : 2;
        int rc = nAxis;

        Log.info({"axis", ra, rb, rc});

        for (int i = 0; i < 2; i++)
            for (int j = i + 1; j < 3; j++)
                if (v[j][ra] < v[i][ra])
                    std::swap(v[i], v[j]);

        Log.info({v[0], v[1], v[2]});

        vec3 a = v[0];
        vec3 b = v[0];

        vec3 e01 = (v[1] - v[0]) / (v[1][ra] - v[0][ra]);
        vec3 e02 = (v[2] - v[0]) / (v[2][ra] - v[0][ra]);
        vec3 e12 = (v[2] - v[1]) / (v[2][ra] - v[1][ra]);

        vec3 d1 = e01;
        float aEnd = v[1][ra];
        if (abs(v[1][ra] - v[0][ra]) < EPS) {
            d1 = e12;
            aEnd = v[2][ra];
            a = v[1];
        }

        vec3 d2 = e02;
        float bEnd = v[2][ra];

        Log.info({"d1, d2:"});

//        a += d1 * (1 - fract(v[0][ra]));
//        b += d2 * (1 - fract(v[0][ra]));

        while (b[ra] <= bEnd) {
            vec3 cur = a;
            vec3 end = b;

            vec3 sideStep = (end - cur) / abs(end[rb] - cur[rb]);
            float s = sign(sideStep[rb]);

            Log.info({"outerStep", cur, end, sideStep, s, s * cur[rb], s * end[rb]});

            while(s * cur[rb] < s * end[rb]) {
                res.push_back(Voxel{ .pos = {cur.x + 0.5f, cur.y + 0.5f, cur.z + 0.5f} });
                cur += sideStep;
            }

            res.push_back(Voxel{ .pos = {cur.x + 0.5f, cur.y + 0.5f, cur.z + 0.5f} });
//            Log.info({sideStep});

            a += d1;
            b += d2;

            if (a[ra] >= aEnd) {
                float overshot = a[ra] - v[1][ra];
                a -= d1 * overshot;

                d1 = e12;
                aEnd = v[2][ra];

                a += d1 * overshot;
            }
        }

        return res;
    }

    void voxelize(const std::vector<Triangle> &triangles) {
        std::unordered_map<uint64_t, Voxel> map;

        for (const auto &tri: triangles) {
            voxelizeTriangle(tri);
        }

    }

    bool Voxel::operator==(const Voxel &other) const {
        return pos == other.pos;
    }
}