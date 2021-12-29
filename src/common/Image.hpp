#pragma once

#include <string>
#include <stb_image.h>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

template<typename T>
struct Image {
    static constexpr size_t TYPE_SIZE = sizeof(T);

    int width;
    int height;
    int numChannels;

    std::vector<uint8_t> image;

    Image(const std::string &filepath) {
        uint8_t *stbiImg = stbi_load(filepath.c_str(), &width, &height, &numChannels, 0);

        if (!stbiImg)
            throw std::runtime_error("Failed to load image " + filepath);

        image.insert(image.end(), stbiImg, stbiImg + width * height * numChannels);

        stbi_image_free(stbiImg);
    }

    Image(int width, int height, int numChannels=0) : width(width), height(height), numChannels(numChannels), image(width * height * TYPE_SIZE) {}

    T get(glm::uvec2 coord) const {
        auto arr = reinterpret_cast<const T*>(image.data());
        return arr[coord.y * width + coord.x];
    }

    T getByTexCoord(glm::vec2 texCoord) const {
        return get(glm::fract(texCoord) * glm::vec2(width, height));
    }
};