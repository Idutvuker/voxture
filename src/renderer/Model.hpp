#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "../data/CompactOctree.hpp"
#include "mygl.hpp"
#include "Texture.hpp"
#include "../util/Logger.hpp"

struct Drawable {
    virtual void draw(const glm::mat4 &MVPMat) const = 0;
};

struct OctreeTexModel : Drawable {
    GLuint VAO;
    GLuint VBO;
    GLuint DAG_SSBO;
    Texture texture{GL_TEXTURE_1D};

    const std::vector<Triangle> &mesh;
    const ShaderProgram &shader;

    OctreeTexModel(const std::vector<Triangle> &_mesh, const ShaderProgram &_shader) : mesh(_mesh), shader(_shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mesh.size() * sizeof(Triangle)), mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &DAG_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, DAG_SSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, DAG_SSBO);
    }

    static int findSide(size_t linSize) {
        return glm::ceil(glm::sqrt(linSize));
    }

    static Image<uint32_t> createImage(const std::vector<uint32_t> &colors) {
        auto side = findSide(colors.size());

        Log.info({colors.size(), side, side * side});

        Image<uint32_t> res(side, side);

        for (size_t i = 0; i < colors.size(); i++) {
            res.image[i] = colors[i];
        }

        return res;
    }

    void updateTree(const CompactOctree &octree) {
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.dag.size() * sizeof(CompactOctree::Node)),
                     octree.dag.data(), GL_DYNAMIC_DRAW);

        texture.bind();
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, GLsizei(octree.colors.size()), 0, GL_RGBA, GL_UNSIGNED_BYTE, octree.colors.data());

//
//        auto img = createImage(octree.colors);
//        img.saveToDisk("colorsTex.bmp");
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

struct UVTexModel : Drawable {
    GLuint VAO;
    GLuint VBO;
    GLuint SSBO;

    const std::vector<TriangleUV> &mesh;
    const ShaderProgram &shader;

    UVTexModel(const std::vector<TriangleUV> &_mesh, const ShaderProgram &_shader) : mesh(_mesh), shader(_shader) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mesh.size() * sizeof(TriangleUV)), mesh.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
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