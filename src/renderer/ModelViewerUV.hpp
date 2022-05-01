#pragma once

#include "GLFWContext.hpp"
#include "Model.hpp"

struct ModelViewerUV {
    Bundle<TexTriangle> bundle;

    GLFWContext context;
    Resources res;

    UVTexModel model{bundle.mesh, res.texModelSP};

    RenderCamera renderCamera{float(context.windowWidth) / float(context.windowHeight)};
    OrbitCameraController cameraController{renderCamera, context};

    Image<glm::u8vec3> atlas;

    explicit ModelViewerUV(const std::string &bundlePath) :
            bundle(bundlePath + "model.ply"),
            atlas(bundlePath + "texture.jpg") {}

    void loadTexture() {
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, atlas.width, atlas.height, 0, GL_RGB, GL_UNSIGNED_BYTE, atlas.image.data());
    }

    void run() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        loadTexture();

        auto prevTime = float(glfwGetTime());
        while (!glfwWindowShouldClose(context.window)) {
            float stamp = float(glfwGetTime());
            float delta = stamp - prevTime;
            prevTime = stamp;

            glfwPollEvents();

            if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(context.window, true);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Begin("Controls");
                ImGui::SliderInt("orbit Rad", &GLFWContext::GLOBAL_SCROLL_Y, -5, 30);

                float fps = 1.f / delta;
                ImGui::Text("FPS: %f %f", fps, delta);

                ImGui::End();
            }

            ImGui::Render();

            cameraController.update(delta);

            glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model.draw(renderCamera.getViewProj());

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(context.window);
        }
    }

    Benchmark benchmark {context};

    void runBenchmark() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        loadTexture();

        benchmark.start(model);
        benchmark.analyze();
    }
};
