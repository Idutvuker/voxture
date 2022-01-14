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

    void update(const std::function<void()> &drawFunc) {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        drawFunc();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, context.WINDOW_WIDTH, context.WINDOW_HEIGHT);

        data.clear();
        data.emplace_back(width, height);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data[0].image.data());

        int curWidth = width / 2;
        int curHeight = height / 2;

        while (curWidth != 1 && curHeight != 1) {
            data.emplace_back(curWidth, curHeight);
            auto curImg = data.back();

        }
    }
};