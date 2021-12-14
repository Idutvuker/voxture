#pragma once

#include "../geom/triangle.hpp"
#include "camera.hpp"

#include <fstream>
#include <filesystem>

struct Bundle {
    std::vector<Triangle> mesh;
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

        for (size_t c = 0; c < nCameras; c++) {
            double focalLength, k1, k2;
            filestream >> focalLength >> k1 >> k2;

            glm::mat3 orientation;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    filestream >> orientation[j][i];
                }
            }

            vec3 temp = orientation[1];
            orientation[1] = orientation[2];
            orientation[2] = -temp;

            glm::vec3 pos;
            for (int i = 0; i < 3; i++) {
                filestream >> pos[i];
            }

            glm::mat4 result = translate(mat4(1), pos) * mat4(orientation);

            Camera cam;
            cam.transform = result;
            cam.focalLength = float(focalLength);

            cameras.push_back(cam);
        }
    }

    void loadImages() {
        std::string listFilepath = "resources/saharov/list.txt";
        std::filesystem::path imgDir= "resources/saharov/imgs";

        std::ifstream filestream(listFilepath);

        for (Camera &camera: cameras) {
            std::string imgName;
            filestream >> imgName;

            camera.photo.emplace(imgDir / imgName);
        }
    }

    void load() {
        getTriangles("resources/saharov/saharov.obj", mesh).assertOK();
        loadCameras();
        loadImages();
    }
};