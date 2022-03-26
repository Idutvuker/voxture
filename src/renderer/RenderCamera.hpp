#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../common/constants.hpp"
#include "../geom/Camera.hpp"

struct RenderCamera : Camera {
    const float aspectRatio;

    const float FOV = CAMERA_DEFAULT_FOV;
    const float NEAR = CAMERA_NEAR;
    const float FAR = CAMERA_FAR;

    explicit RenderCamera(float aspectRatio) :
        Camera{glm::mat4(1), glm::perspective(glm::radians(CAMERA_DEFAULT_FOV), aspectRatio, CAMERA_NEAR, CAMERA_FAR)},
        aspectRatio(aspectRatio)
        {}
};
