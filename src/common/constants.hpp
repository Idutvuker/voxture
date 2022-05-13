#pragma once
#include <cstdint>
#include <array>
#include <glm/glm.hpp>

using uint = uint32_t;

constexpr uint32_t RESOLUTION_LEVEL = 20;

constexpr float CAMERA_NEAR = 0.005f;
constexpr float CAMERA_FAR = 10.f;
constexpr float CAMERA_DEFAULT_FOV = 48.f;

const std::string BENCHMARK_CAMERAS_PATH = "benchmarkCameras.txt";

constexpr std::array<glm::uvec3, 8> VOX_OFFSET = {
        glm::ivec3(0, 0, 0),
        glm::ivec3(0, 0, 1),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 1, 1),

        glm::ivec3(1, 0, 0),
        glm::ivec3(1, 0, 1),
        glm::ivec3(1, 1, 0),
        glm::ivec3(1, 1, 1),
};

constexpr uint voxelOffsetToIndex(const glm::uvec3 &offs) {
    return (offs.x << 2) + (offs.y << 1) + (offs.z);
}