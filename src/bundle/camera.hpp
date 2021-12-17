#pragma once

#include <glm/glm.hpp>

struct Camera {
    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);

    float focalLength = 0;

    std::optional<Image<glm::u8vec3>> photo;
};
