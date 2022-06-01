#pragma once

#include "ModelViewer.hpp"
#include "VoxelGrid.hpp"
#include "DebugDraw.hpp"
#include "../builder/PartialTreeBuilder.hpp"

#include "RenderCamera.hpp"
#include "OrbitCameraController.hpp"
#include "stb_image_write.h"
#include "../bundle/Bundle.hpp"
#include "../data/CompactOctree.hpp"

#include "GLFWContext.hpp"
#include "../test/Benchmark.hpp"
#include "Model.hpp"
#include "../data/DDSImage.hpp"

struct ModelViewerOctree : ModelViewer {
    Bundle<> bundle;

    VoxelGrid voxelGrid;

    CompactOctree octree;
    OctreeTexModel model {bundle.mesh, octree.colors.header.compressed ? res.modelCompColorsSP : res.modelRawColorsSP};

    struct ViewPlane {
        GLuint VAO;
        GLuint VBO;

        const Resources &res;

        ViewPlane(const Resources &_res) : res(_res) {
            std::array<GLfloat, 6 * 3> tris = { -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 0, 1, -1, 0 };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(tris.size() * sizeof(GLfloat)), tris.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);
        }

        void draw() {
            res.imageSP.use();

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    } viewPlane { res };

    enum class DrawMode : int {
        MODEL, VOXELS, VIEW_PLANE
    } drawMode = DrawMode::MODEL;

    void draw() {
        glm::mat4 MVP;
        if (useSavedCamera && !savedCameras.empty()) {
            MVP = savedCameras[savedCameraID];
        } else {
            MVP = renderCamera.getViewProj();
        }

        if (drawMode == DrawMode::MODEL) {
            model.draw(MVP);
        }
        else if (drawMode == DrawMode::VOXELS) {
            voxelGrid.drawCompactOctree(renderCamera, res, octree, drawOctreeLevel);
        } else {
            viewPlane.draw();
        }
    }

    void update(float delta) {
        cameraController.update(delta);
    }

    bool useSavedCamera = false;
    int savedCameraID = 0;

    int drawOctreeLevel = 0;

    Image<glm::u8vec3> img{"resources/textures/test_texture.bmp"};

    void run() override {
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
                ImGui::SliderInt("orbit radius", &GLFWContext::GLOBAL_SCROLL_Y, -5, 30);

                if (ImGui::Button("Draw Model"))
                    drawMode = DrawMode::MODEL;

                if (ImGui::Button("Draw View Plane"))
                    drawMode = DrawMode::VIEW_PLANE;

                if (ImGui::Button("Draw Voxels"))
                    drawMode = DrawMode::VOXELS;

                ImGui::SliderInt("Octree level", &drawOctreeLevel, 0, 10);

                ImGui::Text("Nodes: %zu", octree.dag.size());
                ImGui::Text("Colors: %zu", octree.colors.size());
                ImGui::Text("FPS: %.1f", 1.f / delta);

                if (ImGui::Button("Save camera"))
                    saveCamera();

                ImGui::Checkbox("Use saved camera", &useSavedCamera);
                ImGui::SliderInt("Saved camera id", &savedCameraID, 0, int(savedCameras.size()));

                ImGui::End();
            }

            ImGui::Render();

            update(delta);

            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            draw();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(context.window);
        }
    }

    void saveCamera() {
        std::ofstream output(BENCHMARK_CAMERAS_PATH, std::ios::app);

        glm::mat4 mat = renderCamera.getViewProj();

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                output << mat[i][j] << ' ';
            }
            output << '\n';
        }
    }

    void loadTexture() {
        model.updateTree(octree);
    }

    Benchmark benchmark {context};

    void runBenchmark() override {
        loadTexture();

        benchmark.start(model);
    }


    std::vector<glm::mat4> savedCameras = Benchmark::getCameras();

    ModelViewerOctree(const std::string &bundlePath, const std::string &octreePath) :
            bundle(bundlePath + "model.ply"),
            octree(octreePath)
    {}
};