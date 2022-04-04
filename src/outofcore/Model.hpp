#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "CompactTree.hpp"

struct ModelInterface {
    virtual void updateTree(const CompactTree &octree) = 0;
    virtual void draw(const glm::mat4 &MVPMat) const = 0;
};

struct Model : ModelInterface {
    GLuint VAO;
    GLuint VBO;
    GLuint SSBO;

    const std::vector<Triangle> &mesh;
    const ShaderProgram &shader;

    Model(const std::vector<Triangle> &_mesh, const ShaderProgram &_shader) : mesh(_mesh), shader(_shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mesh.size() * sizeof(Triangle)), mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    }

    void updateTree(const CompactTree &octree) override {
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.data.size() * sizeof(CompactTree::Node)),
                     octree.data.data(), GL_DYNAMIC_DRAW);
    }

    void draw(const glm::mat4 &MVPMat) const override {
        using namespace glm;
        shader.use();

        GLint MVPLoc = glGetUniformLocation(shader.programID, "uModelViewProjMat");

        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * mesh.size()));
    }
};

struct TexturedModel : ModelInterface {
    GLuint VAO;
    GLuint VBO;
    GLuint SSBO;

    const std::vector<TexTriangle> &mesh;
    const ShaderProgram &shader;

    TexturedModel(const std::vector<TexTriangle> &_mesh, const ShaderProgram &_shader) : mesh(_mesh), shader(_shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mesh.size() * sizeof(TexTriangle)), mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    void updateTree(const CompactTree &octree) override {}

    void draw(const glm::mat4 &MVPMat) const override {
        using namespace glm;
        shader.use();

        GLint MVPLoc = glGetUniformLocation(shader.programID, "uModelViewProjMat");

        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * mesh.size()));
    }
};