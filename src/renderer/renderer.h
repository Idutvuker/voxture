#pragma once

#include "camera.h"
#include "cube.h"
#include "../util/Status.h"
#include "../util/Logger.h"
#include "shaderProgram.h"
#include "mygl.h"
#include "../geom/triangle.h"
#include "glfwContext.h"
#include "../geom/voxelizer.h"
#include "resources.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Renderer {
    GLFWContext context;
    Resources res;

    struct RenderData {
        Voxelizer::OctreeLevels &treeLevels;
        std::vector<Triangle> &mesh;
        glm::vec3 center;
    } data;

    GLuint VAO = 0;
    GLuint VBO = 0;

    Camera camera;

    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    GLuint SSBO;

    void initRes() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(data.mesh.size() * sizeof(Triangle)), data.mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        Cube::initCube();

        //GLuint shader_data[2] = {0, 1};

        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), shader_data, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);

//        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void drawModel() const {
        using namespace glm;
        res.modelSP.use();

        mat4 MVPMat = camera.projection * camera.view;

        GLint MVPLoc = glGetUniformLocation(res.modelSP.programID, "uModelViewProjMat");
        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * data.mesh.size()));
    }

    void draw() {
        if (modelMode)
            drawModel();
        else
            Cube::drawVoxels(camera, res, data.treeLevels.data[treeLevel]);
    }

    float rotX = 0;
    float rotY = 0;
    float camRadius = 2.5;
    int treeLevel = 0;
    bool modelMode = false;

    void update(float delta) {
        using namespace glm;

        static mat4 viewBase = translate(mat4(1.0f), -data.center);

        camera.view =
                translate(mat4(1), vec3(0, 0, -camRadius)) *
                glm::rotate(mat4(1), rotY, glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(mat4(1), rotX, glm::vec3(0.0f, 1.0f, 0.0f)) *
                viewBase;
    }

    void mainLoop() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        auto prevTime = float(glfwGetTime());
        while (!glfwWindowShouldClose(context.window)) {
            float delta = float(glfwGetTime()) - prevTime;

            glfwPollEvents();
            processInput(context.window);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Begin("Hello");
                ImGui::SliderFloat("rot X", &rotX, 0.0f, 2 * glm::pi<float>());
                ImGui::SliderFloat("rot Y", &rotY, -glm::half_pi<float>(), glm::half_pi<float>());
                ImGui::SliderFloat("cam Rad", &camRadius, 0.5, 4);

                ImGui::SliderInt("Tree level", &treeLevel, 0, int(data.treeLevels.data.size()) - 1);
                ImGui::Text("Voxels: %d", int(data.treeLevels.data[treeLevel].set.size()));

                ImGui::Checkbox("Draw Model", &modelMode);

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

    explicit Renderer(RenderData _renderData) :
            data(_renderData),
            camera(float(context.WINDOW_WIDTH) / float(context.WINDOW_HEIGHT))
    {
        initRes();
    }
};
