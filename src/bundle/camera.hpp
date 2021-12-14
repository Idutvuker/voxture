#pragma once

#include <glm/glm.hpp>

struct Camera {
    glm::mat4 transform = glm::mat4(1);
    float focalLength = 0;

    std::optional<Image<glm::u8vec3>> photo;
};
