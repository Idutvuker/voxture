#pragma once

#include "../geom/triangle.hpp"
#include "../geom/camera.hpp"

#include <fstream>
#include <glm/gtx/euler_angles.hpp>

struct SaharovLoader {
    std::vector<Triangle> triangles;
    std::vector<Camera> cameras;

    void loadCameras() {
        using namespace glm;

        std::string filepath = "resources/saharov/cameras.out";
        std::ifstream filestream(filepath);

        std::string header_line;
        std::getline(filestream, header_line);
        if (header_line != "# Bundle file v0.3") {
            throw std::runtime_error("Can't import bundler .out file - unexpected header line! " + header_line);
        }

        size_t nCameras, nPoints;
        filestream >> nCameras >> nPoints;

        for (size_t c = 0; c < nCameras / 2; c++) {
            double f, k1, k2;
            filestream >> f >> k1 >> k2;

            glm::mat3 orientation;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    filestream >> orientation[j][i];
                }
            }

            vec3 temp = orientation[1];
            orientation[1] = orientation[2];
            orientation[2] = -temp;


            Log.info({orientation[0], orientation[1], orientation[2]});

            glm::vec3 pos;
            for (int i = 0; i < 3; i++) {
                filestream >> pos[i];
            }

            std::swap(pos[1], pos[2]);
            pos[2] = -pos[2];

            Log.info({"pos", pos});

            glm::mat4 result = translate(mat4(orientation), vec3(0, -14, 0));

            Log.info({result[0], result[1], result[2], result[3]});

            cameras.push_back(Camera{result});
        }
    }

    void load() {
        getTriangles("resources/saharov/saharov.obj", triangles).assertOK();
//        getTriangles("resources/models/directions.obj", triangles).assertOK();
        loadCameras();
    }
};