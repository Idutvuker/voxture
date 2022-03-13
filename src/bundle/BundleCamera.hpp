#pragma once

#include <glm/glm.hpp>
#include "../geom/Camera.hpp"
#include "../common/Image.hpp"
#include <optional>

struct BundleCamera {
    Camera camera;

    float focalLength = 0;

    std::optional<Image<glm::u8vec3>> photo;
};
