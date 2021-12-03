#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

struct RenderCamera {
    const float FOV = 45;
    const float NEAR = 0.1f;
    const float FAR = 100.0f;

    glm::mat4 projection;
    glm::mat4 view;

    explicit RenderCamera(float aspectRatio) :
        projection(glm::perspective(glm::radians(FOV), aspectRatio, NEAR, FAR)),
        view(glm::mat4(1))
    {}

    float orbitRadius = 2.5;
    float rotX = 0;
    float rotY = 0;
    glm::mat4 orbitBase = glm::mat4(1);

    void update(float delta) {
        using namespace glm;

        view = translate(mat4(1), vec3(0, 0, -orbitRadius)) *
               glm::rotate(mat4(1), rotY, glm::vec3(1.0f, 0.0f, 0.0f)) *
               glm::rotate(mat4(1), rotX, glm::vec3(0.0f, 1.0f, 0.0f)) *
               orbitBase;
    }
};
