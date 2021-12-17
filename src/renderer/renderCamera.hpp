#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../common/constants.hpp"

struct RenderCamera {
    const float aspectRatio;
    float FOV = 60;

    const float NEAR = 0.1f;
    const float FAR = 100.0f;

    glm::mat4 projection;
    glm::mat4 view;

    explicit RenderCamera(float aspectRatio) :
        aspectRatio(aspectRatio),
        projection(glm::perspective(glm::radians(FOV), aspectRatio, NEAR, FAR)),
//        projection(glm::ortho(-5.0f, 5.f, -5.f, 5.f, NEAR, FAR)),
        view(glm::mat4(1))
    {}

    void update(float) {
        using namespace glm;

        projection = glm::perspective(glm::radians(FOV), aspectRatio, NEAR, FAR);
    }
};
