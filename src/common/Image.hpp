#pragma once

#include <string>
#include <stb_image.h>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <cstring>

template<typename T>
struct Image {
    static constexpr size_t TYPE_SIZE = sizeof(T);

    int width;
    int height;
    int numChannels;

    std::vector<T> image;

    Image(const std::string &filepath) {
        uint8_t *stbiImg = stbi_load(filepath.c_str(), &width, &height, &numChannels, 0);

        if (!stbiImg)
            throw std::runtime_error("Failed to load image " + filepath);

        image.resize(width * height);
        std::memcpy(image.data(), stbiImg, width * height * TYPE_SIZE);

        stbi_image_free(stbiImg);
    }

    Image(int width, int height, int numChannels=0) : width(width), height(height), numChannels(numChannels), image(width * height) {}

    T& get(glm::uvec2 coord) {
        return image[coord.y * width + coord.x];
    }

    T get(glm::uvec2 coord) const {
        return image[coord.y * width + coord.x];
    }

    T getByTexCoord(glm::vec2 texCoord) const {
        return get(glm::fract(texCoord) * glm::vec2(width, height));
    }

    T getClamped(glm::ivec2 coord) const {
        return get(glm::clamp(coord, glm::ivec2(0, 0), glm::ivec2(width - 1, height - 1)));
    }
};