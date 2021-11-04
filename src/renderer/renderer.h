#pragma once

#include "../util/Status.h"
#include "../util/Logger.h"
#include "shaderProgram.h"
#include "mygl.h"
#include "../geom/triangle.h"
#include "glfwContext.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

struct Renderer {
    GLFWContext context;

    struct RenderData {
        std::vector<Triangle> mesh;
        std::string vertexShaderFilepath;
        std::string fragmentShaderFilepath;
    } renderData;

    GLuint VAO = 0;
    GLuint VBO = 0;
    ShaderProgram shaderProgram;

    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void initRes() {
        GLfloat vertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.0f,  0.5f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw() {
        using namespace glm;

        shaderProgram.use();

        mat4 model = mat4(1.0f);
        mat4 view = mat4(1.0f);
        mat4 projection = glm::perspective(
                glm::radians(45.0f),
                (float)context.WINDOW_WIDTH / (float)context.WINDOW_HEIGHT,
                0.1f, 100.0f
        );
        model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::translate(view, vec3(0, 0, -3));

        GLint modelLoc = glGetUniformLocation(shaderProgram.programID, "model");
        GLint viewLoc  = glGetUniformLocation(shaderProgram.programID, "view");
        GLint projLoc  = glGetUniformLocation(shaderProgram.programID, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

public:
    void mainLoop() {
        while (!glfwWindowShouldClose(context.window)) {
            processInput(context.window);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            draw();

            glfwSwapBuffers(context.window);
            glfwPollEvents();
        }
    }

    explicit Renderer(RenderData _renderData) :
        renderData(std::move(_renderData)),
        shaderProgram(renderData.vertexShaderFilepath, renderData.fragmentShaderFilepath)
    {
        initRes();
    }
};
