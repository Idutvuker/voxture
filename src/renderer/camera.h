#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Camera {
    const float FOV = 45;

    glm::mat4 projection;
    glm::mat4 view;

    explicit Camera(float aspectRatio) :
        projection(glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 100.0f)),
        view(glm::mat4(1))
    {}
};
