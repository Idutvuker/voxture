#pragma once

#include "../renderer/VoxelGrid.hpp"
#include "TreeBuilder.hpp"

#include "DBH.hpp"

struct App {
    Bundle bundle {"resources/testBundle/cow.obj", "resources/testBundle/cameras.out", "resources/testBundle/list.txt"};

    GLFWContext context;
    Resources res;

    VoxelGrid voxelGrid;

    RenderCamera renderCamera {float(context.WINDOW_WIDTH) / float(context.WINDOW_HEIGHT)};
    OrbitCameraController cameraController {renderCamera, context};

    TreeBuilder treeBuilder {bundle.mesh};

    struct Model {
        GLuint VAO;
        GLuint VBO;

        const std::vector<Triangle> &mesh;
        const Resources &res;

        Model(const std::vector<Triangle> &_mesh, const Resources &_res) : mesh(_mesh), res(_res) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mesh.size() * sizeof(Triangle)), mesh.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);
        }

        void draw(const glm::mat4 &MVPMat) const {
            using namespace glm;
            res.testSP.use();

            GLint MVPLoc = glGetUniformLocation(res.testSP.programID, "uModelViewProjMat");

            glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

            glBindVertexArray(VAO);

            glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * mesh.size()));
        }
    } model {bundle.mesh, res};

    void update(float delta) {
        cameraController.update(delta);
    }

    bool drawMode = true;

    void draw() {
        if (drawMode)
            model.draw(renderCamera.projection * renderCamera.view);
        else
            voxelGrid.drawFromVec(renderCamera, res, treeBuilder.voxels, (1 << treeBuilder.maxLevel));
    }

    int maxLevelWrapper = int(treeBuilder.maxLevel);

    void run() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

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

                ImGui::Checkbox("Draw Model", &drawMode);

                if (ImGui::SliderInt("Level", &maxLevelWrapper, 0, 10))
                    treeBuilder.maxLevel = uint(maxLevelWrapper);

                ImGui::Text("Voxel count: %zu", treeBuilder.voxels.size());

                if (ImGui::Button("Rebuild tree"))
                    treeBuilder.buildTree(renderCamera);

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

    App() = default;
};