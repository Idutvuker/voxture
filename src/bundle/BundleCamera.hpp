#pragma once

#include <glm/glm.hpp>
#include "../geom/Camera.hpp"
#include "../common/Image.hpp"
#include "PhotoInfo.hpp"
#include <optional>

struct BundleCamera {
    Camera camera;

    float focalLength = 0;

    PhotoInfo photoInfo;
};
