#pragma once

#include <glm/glm.hpp>
#include "../common/constants.hpp"

struct Camera {
    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);

    glm::mat4 getViewProj() const {
        return projection * view;
    }

    static float linearizeDepthNDC(float depth) {
        return (2.f * CAMERA_NEAR * CAMERA_FAR) / (CAMERA_FAR + CAMERA_NEAR - depth * (CAMERA_FAR - CAMERA_NEAR));
    }
};