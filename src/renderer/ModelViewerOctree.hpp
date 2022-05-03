#pragma once

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

struct ModelViewerOctree {
    Bundle<> bundle;

    GLFWContext context {900, 600};
    Resources res;

    VoxelGrid voxelGrid;

    RenderCamera renderCamera {float(context.windowWidth) / float(context.windowHeight)};
    OrbitCameraController cameraController {renderCamera, context};

    OctreeTexModel model {bundle.mesh, res.modelSP};

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
//            voxelGrid.drawOctree(renderCamera, res, rawOctree);
            model.draw(MVP);
        }
        else if (drawMode == DrawMode::VOXELS) {
            voxelGrid.drawCompactOctree(renderCamera, res, octree);
        }
    }

    void update(float delta) {
        cameraController.update(delta);
    }

    bool useBundleCamera = false;

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

                {
                    if (ImGui::Button("Draw Model"))
                        drawMode = DrawMode::MODEL;

                    if (ImGui::Button("Draw View Plane"))
                        drawMode = DrawMode::VIEW_PLANE;

                    if (ImGui::Button("Draw Voxels"))
                        drawMode = DrawMode::VOXELS;

                    ImGui::Checkbox("Bundle camera", &useBundleCamera);
                }

                ImGui::Text("Voxel count: %zu", octree.dag.size());

                if (ImGui::Button("Load tree"))
                    loadTexture();

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

    CompactOctree octree;
    RawOctree rawOctree;

    void loadTexture() {
        model.updateTree(octree);
    }

    ModelViewerOctree(const std::string &bundlePath, const std::string &octreePath) :
        bundle(bundlePath + "model.ply"),
        rawOctree(octreePath)
    {
        octree = CompactOctreeBuilder(rawOctree).output;
        std::cout << "SIZE: " << octree.dag.size() << std::endl;
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