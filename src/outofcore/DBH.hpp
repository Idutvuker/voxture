#pragma once

#include <functional>

// Depth Buffer Hierarchy
struct DBH {
    std::vector<Image<float>> data;
    const GLFWContext &context;

    GLuint depthTex = 0;
    GLuint depthFBO = 0;

    int width;
    int height;

    DBH(const GLFWContext &_context, int _width, int _height): context(_context), width(_width), height(_height) {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1, &depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void update(const std::function<void(const glm::mat4&)> &drawFunc, const glm::mat4& MVPMat) {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        drawFunc(MVPMat);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, context.WINDOW_WIDTH, context.WINDOW_HEIGHT);

        data.clear();
        data.emplace_back(width, height);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data.back().image.data());

        int curWidth = (width + 1) / 2;
        int curHeight = (height + 1) / 2;

        while (true) {
            data.emplace_back(curWidth, curHeight);

            auto &prevImg = data[data.size() - 2];
            auto &curImg = data.back();

            for (uint y = 0; y < uint(curHeight); y++) {
                for (uint x = 0; x < uint(curWidth); x++) {
                    float maxDepth = glm::max(
                            glm::max(prevImg.getClamped({x * 2,      y * 2}),
                                     prevImg.getClamped({x * 2 + 1,  y * 2})),
                            glm::max(prevImg.getClamped({x * 2,      y * 2 + 1}),
                                     prevImg.getClamped({x * 2 + 1,  y * 2 + 1})
                            )
                    );

                    curImg.get({x, y}) = maxDepth;
                }
            }

            if (curWidth == 1 && curHeight == 1)
                break;

            curWidth = (curWidth + 1) / 2;
            curHeight = (curHeight + 1) / 2;
        }
    }

    static constexpr std::array<glm::uvec2, 4> PIXEL_OFFSET = {
            glm::uvec2(0, 0),
            glm::uvec2(0, 1),
            glm::uvec2(1, 0),
            glm::uvec2(1, 1),
    };

    static int aabbInside(const glm::uvec2 &innerMin, const glm::uvec2 &innerMax, const glm::uvec2 &outerMin, const glm::uvec2 &outerMax) {
        using namespace glm;
        if (all(lessThanEqual(outerMin, innerMin))) {
            if (all(lessThanEqual(innerMax, outerMax)))
                return 2; // inner inside outer
            if (all(lessThanEqual(innerMin, outerMax)))
                return 1; // intersect
            return 0; // do not intersect
        }

        return all(lessThan(innerMin, outerMax)) && all(lessThan(outerMin, innerMax)) ? 1 : 0;
    }

    float getMax(uint level, const glm::uvec2 &pixel, const glm::uvec2 &from, const glm::uvec2 &to) const {
        using namespace glm;

        auto &img = data[data.size() - level - 1];

        auto dims = uvec2(img.width, img.height);
        if (any(greaterThanEqual(pixel, dims)))
            return 0;

        uint levelsLeft = uint(data.size() - level - 1);

        uvec2 pMin = pixel << levelsLeft;
        uvec2 pMax = ((pixel + uvec2(1)) << levelsLeft) - uvec2(1);

        int aabbTest = aabbInside(pMin, pMax, from, to);

        if (aabbTest == 0)
            return 0;

        if (aabbTest == 2 || level == data.size() - 1)
            return img.get(pixel);

        float res = 0;
        for (const auto &offs: PIXEL_OFFSET) {
            res = max(getMax(level + 1, pixel * uint(2) + offs, from, to), res);
        }

        return res;
    }

    float queryMax(const glm::vec2 &rangeMin, const glm::vec2 &rangeMax) const {
        using namespace glm;

        uvec2 bounds(width - 1, height - 1);
        vec2 dims(width, height);

        uvec2 from = min(uvec2(dims * rangeMin), bounds);
        uvec2 to = min(uvec2(dims * rangeMax), bounds);

        return getMax(0, glm::uvec2(0), from, to);
    }

    void debugLevel(int level) {
        if (level < 0 || level >= int(data.size()))
            return;

        auto &img = data[level];

        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, img.width, img.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, img.image.data());
    }
};