#pragma once

#include "../geom/triangle.hpp"
#include "../geom/camera.hpp"

#include <fstream>

struct SaharovLoader {
    std::vector<Triangle> triangles;
    std::vector<Camera> cameras;

    void loadCameras() {
        std::string filepath = "resources/saharov/cameras.out";
        std::ifstream filestream(filepath);

        std::string header_line;
        std::getline(filestream, header_line);
        if (header_line != "# Bundle file v0.3") {
            throw std::runtime_error("Can't import bundler .out file - unexpected header line! " + header_line);
        }

        size_t nCameras, nPoints;
        filestream >> nCameras >> nPoints;

        for (size_t c = 0; c < 1; c++) {
            double f, k1, k2;
            filestream >> f >> k1 >> k2;

            glm::mat3 orientation;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    filestream >> orientation[i][j];
                }
            }

            orientation[1] = -orientation[1];
            orientation[2] = -orientation[2];

            glm::vec3 pos;
            for (int i = 0; i < 3; i++) {
                filestream >> pos[i];
            }

            Log.info({pos});

            pos = glm::inverse(orientation) * (-pos);

            Log.info({pos});
        }
    }

    void load() {
//        getTriangles("resources/saharov/saharov.obj", triangles).assertOK();
        getTriangles("resources/models/cow.obj", triangles).assertOK();
        loadCameras();
    }
};