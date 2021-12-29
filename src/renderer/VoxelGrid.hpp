#pragma once

#include "mygl.hpp"
#include "../geom/Triangle.hpp"
#include "../geom/Camera.hpp"
#include "ShaderProgram.hpp"
#include "Renderer.hpp"
#include "../geom/voxelizer.hpp"
#include "../util/Logger.hpp"
#include "Resources.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

struct VoxelGrid {
    static constexpr float vertices[] = {
            0, 0, 0,
            1, 0, 0,
            1, 0, 1,
            0, 0, 1,

            0, 1, 0,
            1, 1, 0,
            1, 1, 1,
            0, 1, 1
    };

    static constexpr GLuint indices[] = {
            0, 4, 1,
            1, 4, 5,

            1, 5, 2,
            2, 5, 6,

            2, 6, 7,
            2, 7, 3,

            3, 7, 0,
            0, 7, 4,

            4, 7, 6,
            6, 5, 4,

            0, 2, 3,
            0, 1, 2
    };

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    VoxelGrid() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);
    }

    void draw(const Camera &camera, const Resources &res, const Voxelizer::VoxelSet &voxelSet, const Voxelizer::VoxelColors &colors) {
        using namespace glm;

        glBindVertexArray(VAO);

        res.voxelSP.use();
        GLint MVPLoc = glGetUniformLocation(res.voxelSP.programID, "uModelViewProjMat");
        GLint ColorLoc = glGetUniformLocation(res.voxelSP.programID, "uColor");

        const float voxelSize = 1.f / float(voxelSet.getGridSize());

        mat4 ViewProjMat = camera.projection * camera.view;
        mat4 base = scale(mat4(1), vec3(voxelSize));

        for (const auto &voxel: voxelSet.set) {
            vec3 pos = vec3(voxel.pos);
            mat4 model = translate(base, pos);
            mat4 MVPMat = ViewProjMat * model;

            glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, value_ptr(MVPMat));

//            vec3 color = pos * voxelSize;
//            glUniform3fv(ColorLoc, 1, value_ptr(color));

            if (auto it = colors.find(voxel); it != colors.end()) {
                vec3 color = vec3(it->second) / 255.f;
                glUniform3fv(ColorLoc, 1, value_ptr(color));
            }

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }
    }
};