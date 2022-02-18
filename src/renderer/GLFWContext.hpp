#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "mygl.hpp"

struct GLFWContext {
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;

    GLFWwindow *window;

    static void framebuffer_size_callback(GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
    }

    static int GLOBAL_SCROLL_Y;

    static void scrollCallback(GLFWwindow*, double, double dy) {
        GLOBAL_SCROLL_Y += int(dy);
    }

    GLFWContext() {
        if (!glfwInit())
            throw std::runtime_error("GLFW failed to initialize");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Voxture", nullptr, nullptr);
        if (window == nullptr)
            throw std::runtime_error("GLFW Window failed to initialize");

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetScrollCallback(window, scrollCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("GLAD failed to initialize");

        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ~GLFWContext() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }
};

int GLFWContext::GLOBAL_SCROLL_Y = 0;
