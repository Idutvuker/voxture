#pragma once

#include <glm/glm.hpp>

struct Camera {
    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);

//    Camera(const glm::mat4 &view, const glm::mat4 &projection) : view(view), projection(projection) {}
};