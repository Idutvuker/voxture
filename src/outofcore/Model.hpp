#pragma once

#include <glm/gtc/type_ptr.hpp>

struct Model {
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

    void updateTree(const Octree &octree) {
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.data.size() * sizeof(Octree::Node)),
                     octree.data.data(), GL_DYNAMIC_DRAW);
    }

    void draw(const glm::mat4 &MVPMat) const {
        using namespace glm;
        shader.use();

        GLint MVPLoc = glGetUniformLocation(shader.programID, "uModelViewProjMat");

        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * mesh.size()));
    }
};