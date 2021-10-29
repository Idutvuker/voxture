#include "voxelizer.h"
#include "Logger.h"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

namespace Voxelizer {
    std::vector<Voxel> voxelizeTriangle2(const Triangle &tri) {
        const float EPS = 1e-5;

        using namespace glm;

        std::vector<Voxel> res;
        vec3 v[3];

        for (size_t i = 0; i < 3; i++)
            v[i] = tri[i] * float(GRID_SIZE);

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
                res.push_back(Voxel{ .pos = cur + vec3(0.5) });
                cur += sideStep;
            }

            res.push_back(Voxel{ .pos = cur + vec3(0.5f) });
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

    std::vector<Voxel> voxelizeTriangle(const Triangle &tri) {
        using namespace glm;
        std::vector<Voxel> res;

        vec3 v[3];

        for (int i = 0; i < 3; i++)
            v[i] = tri[i] * float(GRID_SIZE);

        vec3 anorm = abs(normalize(cross(tri[1] - tri[0], tri[2] - tri[0])));
        int nAxis = 0;
        for (int i = 1; i < 3; i++)
            if (anorm[i] > anorm[nAxis])
                nAxis = i;

        printf("nAxis = %d\n", nAxis);
        Log.info({"v1", v[0], v[1], v[2]});

        int ra = nAxis == 1 ? 0 : 1; // raster axis
        int rb = nAxis == 2 ? 0 : 2;
        int rc = nAxis;

        Log.info({"axis", ra, rb, rc});

        for (int i = 0; i < 2; i++)
            for (int j = i + 1; j < 3; j++)
                if (v[j][ra] < v[i][ra])
                    std::swap(v[i], v[j]);


        Log.info({"sorted", v[0], v[1], v[2]});

        vec3 d01 = (v[1] - v[0]) / (v[1][ra] - v[0][ra]);
        vec3 d02 = (v[2] - v[0]) / (v[2][ra] - v[0][ra]);
        vec3 d12 = (v[2] - v[1]) / (v[2][ra] - v[1][ra]);

        Log.info({"!!!", d01, d02, d12});

        int xCur = floor(v[0][ra]);

        vec3 a = v[0];
        int aEnd = ceil(v[1][ra]);

        vec3 b = v[0];
        int bEnd = ceil(v[2][ra]);

        while (xCur <= bEnd) {
            int yCur = floor(min(a[rb], b[rb]));
            int yEnd = ceil(max(a[rb], b[rb]));

            if (a[rb] < b[rb]) {
                yCur = floor(a[rb]);
                yEnd = ceil(b[rb]);
            } else {
                yCur = floor(b[rb]);
                yEnd = ceil(a[rb]);
            }
//            Log.info({a[rb], b[rb], yCur, yEnd});

            float zCur = a[rc];
            float zStep = (b[rc] - a[rc]) / (yEnd - yCur);

            while (yCur <= yEnd) {
                uvec3 pos;
                pos[ra] = xCur;
                pos[rb] = yCur;
                pos[rc] = zCur + 0.5f;

                Log.info({"cur", pos});

                res.push_back(Voxel{pos});
                yCur++;
                zCur += zStep;
            }

            if (xCur < aEnd)
                a += d01;
            else
                a += d12;

            b += d02;

            xCur++;
        }


//        res.push_back(Voxel{v[0]});
//        res.push_back(Voxel{v[1]});
//        res.push_back(Voxel{v[2]});
//        Log.info({"v2", v[0], v[1], v[2]});

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

    size_t VoxelHash::operator()(const Voxel &voxel) const {
        return ((voxel.pos.x << 20) + (voxel.pos.y << 10) + voxel.pos.z);
    }
}