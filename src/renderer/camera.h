#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Camera {
    const float FOV = 45;
    const float NEAR = 0.1f;
    const float FAR = 100.0f;

    glm::mat4 projection;
    glm::mat4 view;

    explicit Camera(float aspectRatio) :
        projection(glm::perspective(glm::radians(FOV), aspectRatio, NEAR, FAR)),
        view(glm::mat4(1))
    {}
};
