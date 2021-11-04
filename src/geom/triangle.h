#pragma once

#include <glm/vec3.hpp>

struct Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    glm::vec3& operator[](size_t index);

    const glm::vec3& operator[](size_t index) const;
};