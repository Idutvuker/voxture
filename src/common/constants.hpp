#pragma once
#include <cstdint>
#include <array>
#include <glm/glm.hpp>

constexpr uint32_t RESOLUTION_LEVEL = 10;

constexpr float CAMERA_NEAR = 0.1f;
constexpr float CAMERA_FAR = 100.f;
constexpr float CAMERA_DEFAULT_FOV = 60.f;

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