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
    GLuint COLORS_SSBO;

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
        glGenBuffers(1, &COLORS_SSBO);
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

    Texture texture {GL_TEXTURE_2D};

    void loadCompressedColors(const CompactOctree &octree) {
        texture.bind();

        auto blocksX = octree.colors.header.blocksX;
        auto blocksY = octree.colors.header.blocksY;

        Log.info({blocksX * blocksY, blocksX, blocksY, octree.dag.size(), octree.colors.size()});

        auto width = GLsizei(blocksX * 4);
        auto height = GLsizei(blocksY * 4);

        GLenum format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

        glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GLsizei(octree.colors.size()), octree.colors.data());
        glCheckError();
    }

    void updateTree(const CompactOctree &octree) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, DAG_SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     GLsizeiptr(octree.dag.size() * sizeof(CompactOctree::Node)),
                     octree.dag.data(), GL_DYNAMIC_DRAW);


        if (octree.colors.header.compressed) {
            loadCompressedColors(octree);
        } else {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, COLORS_SSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                         GLsizeiptr(octree.colors.size() * sizeof(uint8_t)),
                         octree.colors.data(), GL_DYNAMIC_DRAW);
        }
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