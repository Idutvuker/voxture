#pragma once

#include <string>
#include <utility>
#include <glm/glm.hpp>

struct PhotoInfo {
    std::string photoPath;
    glm::uvec2 dims;

    PhotoInfo() = default;

    explicit PhotoInfo(std::string _photoPath) : photoPath(std::move(_photoPath)) {
        int x, y, comp;
        stbi_info(photoPath.c_str(), &x, &y, &comp);

        assert(comp == 3);

        dims = glm::vec2(x, y);
    }

    Image<glm::u8vec3> loadImage() const {
        return Image<glm::u8vec3>(photoPath);
    }
};