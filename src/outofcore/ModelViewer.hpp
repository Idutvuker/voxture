#pragma once

#include "../renderer/VoxelGrid.hpp"
#include "../renderer/DebugDraw.hpp"
#include "TreeBuilderRays.hpp"

#include "DBH.hpp"
#include "../renderer/RenderCamera.hpp"
#include "../renderer/OrbitCameraController.hpp"
#include "Model.hpp"
#include "stb_image_write.h"
#include "../bundle/Bundle.hpp"
#include "CompactTree.hpp"

#include "../test/Benchmark.hpp"

struct ModelViewer {
    Bundle<> bundle;

    GLFWContext context;
    Resources res;

    VoxelGrid voxelGrid;

    RenderCamera renderCamera {float(context.windowWidth) / float(context.windowHeight)};
    OrbitCameraController cameraController {renderCamera, context};

    Model model {bundle.mesh, res.modelSP};

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

    int maxLevelWrapper = 0;
    int DBHLevel = 0;

    DebugDraw debugDraw {};

    void draw() {
        glm::mat4 MVP;
        if (useBundleCamera) {
            MVP = bundle.cameras[0].camera.getViewProj();
        } else {
            MVP = renderCamera.getViewProj();
        }

        if (drawMode == DrawMode::MODEL) {
            model.draw(MVP);
//            model.draw(bundle.cameras.front().camera.getViewProj());
        }
        else if (drawMode == DrawMode::VOXELS) {
//            voxelGrid.drawOctree(renderCamera, res, octrees[bundleCameraID]);
//            voxelGrid.drawFromVec(renderCamera, res, (1 << treeBuilder.maxLevel), treeBuilder.voxels);
//            voxelGrid.drawOctree(renderCamera, res, octree);
        }
        else {
            viewPlane.draw();
        }
    }

    void update(float delta) {
        cameraController.update(delta);
    }

    bool useBundleCamera = false;

    void run() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        loadTree();

        auto prevTime = float(glfwGetTime());
        while (!glfwWindowShouldClose(context.window)) {
            float delta = float(glfwGetTime()) - prevTime;

            glfwPollEvents();

            if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(context.window, true);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Begin("Controls");
                ImGui::SliderInt("orbit Rad", &GLFWContext::GLOBAL_SCROLL_Y, -5, 30);

                {
                    if (ImGui::Button("Draw Model"))
                        drawMode = DrawMode::MODEL;

                    if (ImGui::Button("Draw View Plane"))
                        drawMode = DrawMode::VIEW_PLANE;

                    if (ImGui::Button("Draw Voxels"))
                        drawMode = DrawMode::VOXELS;

                    ImGui::Checkbox("Bundle camera", &useBundleCamera);
                }

                ImGui::Text("Voxel count: %zu", octree.data.size());

                if (ImGui::Button("Load tree"))
                    loadTree();

                ImGui::End();
            }

            ImGui::Render();

            update(delta);

            glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            draw();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(context.window);
        }
    }

    CompactTree octree;

    void loadTree() {
        model.updateTree(octree);
    }

    ModelViewer(const std::string &bundlePath, const std::string &octreePath) :
        bundle(bundlePath + "model.ply"),
        octree(octreePath)
    {}


    Benchmark benchmark {context};

    void runBenchmark() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        loadTree();

        benchmark.start(model);
        benchmark.analyze();
    }
};