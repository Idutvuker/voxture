#pragma once

#include "../geom/triangle.hpp"
#include "BundleCamera.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

#include <glm/gtx/matrix_decompose.hpp>

namespace fs = std::filesystem;

struct Bundle {
    std::vector<Triangle> mesh;
    std::vector<BundleCamera> cameras;

    Bundle(const fs::path &meshFilepath, const fs::path &bundleOutFilepath, const fs::path &listFilepath, bool normalize = true) {
        getTriangles(meshFilepath.string(), mesh).assertOK();
        loadCameras(bundleOutFilepath);
        loadImages(listFilepath);

        if (normalize)
            normalizeBundle();
    }

    void loadCameras(const fs::path &bundleOutFilepath) {
        using namespace glm;

        std::ifstream filestream(bundleOutFilepath);

        std::string header_line;
        std::getline(filestream, header_line);
        if (header_line != "# Bundle file v0.3") {
            throw std::runtime_error("Can't import bundler .out file - unexpected header line! " + header_line);
        }

        size_t nCameras, nPoints;
        filestream >> nCameras >> nPoints;

        for (size_t c = 0; c < 2; c++) {
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

            BundleCamera cam;
            cam.camera.view = result;
            cam.focalLength = float(focalLength);

            cameras.push_back(cam);
        }
    }

    void loadImages(const fs::path &listFilepath) {
        std::filesystem::path imgDir = listFilepath.parent_path() / "imgs";

        std::ifstream filestream(listFilepath);

        size_t imgId = 1;

        for (BundleCamera &camera: cameras) {
            printf("\rLoading image: %zu \\ %zu", imgId, cameras.size());
            fflush(stdout);
            imgId++;

            std::string imgName;
            filestream >> imgName;

            camera.photo.emplace(imgDir / imgName);

            float fovY = atan2f(float(camera.photo->height) / 2, camera.focalLength) * 2;

            camera.camera.projection = glm::perspective(fovY, float(camera.photo->width) / float(camera.photo->height), CAMERA_NEAR, CAMERA_FAR);
        }

        printf("\rLoading images done!\n");
    }

    glm::mat4 calcNormalizeMat() {
        auto aabbMin = glm::vec3(std::numeric_limits<float>::infinity());
        auto aabbMax = glm::vec3(-std::numeric_limits<float>::infinity());

        for (auto &tri: mesh) {
            for (int i = 0; i < 3; i++) {
                aabbMin = min(aabbMin, tri[i]);
                aabbMax = max(aabbMax, tri[i]);
            }
        }

        auto diff = aabbMax - aabbMin;

        int scaleAxis = 0;
        for (int i = 1; i < 3; i++)
            if (diff[i] > diff[scaleAxis])
                scaleAxis = i;

        int resolution = 1 << RESOLUTION_LEVEL;
        float scale = std::nextafter(float(resolution - 1) / float(resolution), 0.f);

        return glm::translate(glm::scale(glm::mat4(1), glm::vec3(scale / diff[scaleAxis])), -aabbMin);
    }

    //// Normalizes mesh and cameras
    void normalizeBundle() {
        glm::mat4 normMat = calcNormalizeMat();

        for (auto &t: mesh) {
            for (size_t i = 0; i < 3; i++) {
                t[i] = normMat * glm::vec4(t[i], 1);

                for (int j = 0; j < 3; j++)
                    assert(0 <= t[i][j] && t[i][j] < 1 && "coordinates should be in [0, 1)");
            }
        }

        glm::mat4 inverseNormMat = glm::inverse(normMat);

        for (auto &camera: cameras)
            camera.camera.view *= inverseNormMat;
    }
};