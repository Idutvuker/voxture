#pragma once

#include "../geom/Triangle.hpp"
#include "BundleCamera.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

namespace fs = std::filesystem;

template <typename TriType = Triangle>
struct Bundle {
    std::vector<TriType> mesh;
    std::vector<BundleCamera> cameras;

    explicit Bundle(const fs::path &meshFilepath) : Bundle(meshFilepath, fs::path(), fs::path()) {};

    Bundle(const fs::path &meshFilepath, const fs::path &bundleOutFilepath, const fs::path &listFilepath, bool normalize = true) {
        if constexpr (std::is_same_v<TriType, Triangle>)
            getTriangles(meshFilepath.string(), mesh).assertOK();
        else
            getTexTriangles(meshFilepath.string(), mesh).assertOK();

        if (!bundleOutFilepath.empty())
            loadCameras(bundleOutFilepath);

        if (!listFilepath.empty())
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

        for (size_t c = 0; c < nCameras; c++) {
            double focalLength, k1, k2;
            filestream >> focalLength >> k1 >> k2;

            glm::mat3 orientation;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    filestream >> orientation[j][i];
                }
            }

//            vec3 temp = orientation[1];
//            orientation[1] = orientation[2];
//            orientation[2] = -temp;

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

        for (BundleCamera &camera: cameras) {
            std::string imgName;
            filestream >> imgName;

            camera.photoInfo = PhotoInfo((imgDir / imgName).string());

            float fovY = atan2f(float(camera.photoInfo.dims.y) / 2, camera.focalLength) * 2;

            camera.camera.projection = glm::perspective(fovY, float(camera.photoInfo.dims.x) / float(camera.photoInfo.dims.y), CAMERA_NEAR, CAMERA_FAR);
        }
    }

    std::pair<glm::vec3, float> calcNormalizeParams() {
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
        float unit = std::nextafter(float(resolution - 1) / float(resolution), 0.f);
        float scale = unit / diff[scaleAxis];

        return {-aabbMin, scale};
    }

    //// Normalizes mesh and cameras
    void normalizeBundle() {
        auto normResult = calcNormalizeParams();
        glm::vec3 offset = normResult.first;
        float scale = normResult.second;

        for (auto &tri: mesh) {
            for (size_t i = 0; i < 3; i++) {
                tri[i] = (tri[i] + offset) * scale;

                for (int j = 0; j < 3; j++)
                    assert(0 <= tri[i][j] && tri[i][j] < 1 && "coordinates should be in [0, 1)");
            }
        }

        for (auto &camera: cameras) {
            camera.camera.view = glm::translate(camera.camera.view, -offset);
            camera.camera.view[3].x *= scale;
            camera.camera.view[3].y *= scale;
            camera.camera.view[3].z *= scale;
        }
    }
};