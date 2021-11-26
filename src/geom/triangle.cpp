#include "triangle.hpp"

glm::vec3 &Triangle::operator[](size_t index) {
    if (index == 0)
        return a;
    if (index == 1)
        return b;
    return c;
}

const glm::vec3 &Triangle::operator[](size_t index) const {
    if (index == 0)
        return a;
    if (index == 1)
        return b;
    return c;
}
