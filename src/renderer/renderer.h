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
#include <stb_image.h>

struct Renderer {
    GLFWContext context;
    Resources res;

    struct RenderData {
        Voxelizer::Octree &treeLevels;
        std::vector<Triangle> &mesh;
        glm::vec3 center;
    } shared;

    GLuint VAO = 0;
    GLuint VBO = 0;

    Camera camera;

    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    GLuint SSBO = 0;

    void drawModel() const {
        using namespace glm;
        res.modelSP.use();

        mat4 MVPMat = camera.projection * camera.view;

        GLint MVPLoc = glGetUniformLocation(res.modelSP.programID, "uModelViewProjMat");
        GLint LevelLoc = glGetUniformLocation(res.modelSP.programID, "uLevel");

        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));
        glUniform1ui(LevelLoc, treeLevel);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * shared.mesh.size()));
    }

    GLuint imageVAO, imageVBO;

    struct ImageData {
        glm::u8vec3 *image;
        int width;
        int height;

        glm::u8vec3 get(uint x, uint y) {
            x = x % width;
            y = y % height;
            return image[y * width + x];
        };
    } imageData;

    void initImage() {
        std::array<GLfloat, 6 * 3> tris = {
                -1, 1, 0,
                -1, -1, 0,
                1, 1, 0,

                1, 1, 0,
                -1, -1, 0,
                1, -1, 0,
        };

        glGenVertexArrays(1, &imageVAO);
        glGenBuffers(1, &imageVBO);

        glBindVertexArray(imageVAO);

        glBindBuffer(GL_ARRAY_BUFFER, imageVBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(tris.size() * sizeof(GLfloat)), tris.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        GLuint texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::string filepath = "resources/textures/test.jpg";
        int imChannels;

        uint8_t *data = stbi_load(filepath.c_str(), &imageData.width, &imageData.height, &imChannels, 0);
        if (!data)
            throw std::runtime_error("Failed to load image " + filepath);

//        printf("%u %u %u", data[0], data[1], data[2]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageData.width, imageData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);

        imageData.image = reinterpret_cast<glm::u8vec3 *>(data);

    }

    void drawImage() {
        glBindVertexArray(imageVAO);

        res.imageSP.use();

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void draw() {
        if (imageMode)
            drawImage();
        else {
            if (modelMode)
                drawModel();
            else
                Cube::drawVoxels(camera, res, shared.treeLevels.levels[treeLevel], shared.treeLevels.colors);
        }
    }

    float rotX = 0;
    float rotY = 0;
    float camRadius = 2.5;
    int treeLevel = 0;
    bool modelMode = false;
    bool imageMode = false;

    void update(float delta) {
        using namespace glm;

        static mat4 viewBase = translate(mat4(1.0f), -shared.center);

        camera.view =
                translate(mat4(1), vec3(0, 0, -camRadius)) *
                glm::rotate(mat4(1), rotY, glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(mat4(1), rotX, glm::vec3(0.0f, 1.0f, 0.0f)) *
                viewBase;
    }

    void project() {
        using namespace glm;

        auto &lastLevel = shared.treeLevels.levels.back();
        auto &colors = shared.treeLevels.colors;

        for (const auto &voxel: lastLevel.set) {
            vec3 P = vec3(voxel.pos) / float(lastLevel.getGridSize());
            vec4 S = camera.projection * camera.view * vec4(P, 1.0);
            S /= S.w;

            vec2 texCoord = fract((vec2(S.x, -S.y) + 1.f) / 2.f);

            uvec2 pixelCoord = texCoord * vec2(imageData.width, imageData.height) + 0.5f;

//            u8vec3 color(texCoord * 255.f, 0);

            colors[voxel] = imageData.get(pixelCoord.x, pixelCoord.y);;
        }

        shared.treeLevels.buildRaw();

        auto &octree = shared.treeLevels.rawData;
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.size() * sizeof(Voxelizer::Octree::Node)),
                     octree.data(), GL_DYNAMIC_DRAW);
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

                ImGui::SliderInt("Tree level", &treeLevel, 0, int(shared.treeLevels.levels.size()) - 1);
                ImGui::Text("Voxels: %d", int(shared.treeLevels.levels[treeLevel].set.size()));

                ImGui::Checkbox("Draw Model", &modelMode);
                ImGui::Checkbox("Draw Image", &imageMode);

                if (ImGui::Button("Project"))
                    project();

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

    void initModel() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(shared.mesh.size() * sizeof(Triangle)), shared.mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        auto &octree = shared.treeLevels.rawData;

        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.size() * sizeof(Voxelizer::Octree::Node)),
                     octree.data(), GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    }



    void initRes() {
        initModel();
        Cube::initCube();

        initImage();
    }

    explicit Renderer(RenderData _renderData) :
            shared(_renderData),
            camera(float(context.WINDOW_WIDTH) / float(context.WINDOW_HEIGHT))
    {
        initRes();
    }

    ~Renderer() {
        stbi_image_free(imageData.image);
    }
};
