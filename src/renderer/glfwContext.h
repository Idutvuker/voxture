#pragma once

#include "mygl.h"

struct GLFWContext {
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;

    GLFWwindow *window;

    static void framebuffer_size_callback(GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
    }

    GLFWContext() {
        if (!glfwInit())
            throw std::runtime_error("GLFW failed to initialize");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Voxture", nullptr, nullptr);
        if (window == nullptr)
            throw std::runtime_error("Window failed to initialize");

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("GLAD failed to initialize");

        //        glfwSwapInterval(1);
    }

    ~GLFWContext() {
        glfwTerminate();
    }
};
