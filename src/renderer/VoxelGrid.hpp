#pragma once

#include "mygl.hpp"
#include "../geom/Triangle.hpp"
#include "../geom/Camera.hpp"
#include "ShaderProgram.hpp"
#include "../geom/voxelizer.hpp"
#include "../util/Logger.hpp"
#include "Resources.hpp"
#include "../outofcore/Octree.hpp"
#include "../common/constants.hpp"
#include "../outofcore/CompactTree.hpp"

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

    void draw(const Camera &camera, const Resources &res, const Voxelizer::VoxelSet &voxelSet,
              const Voxelizer::VoxelColors &colors) {
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

    void drawFromVec(const Camera &camera, const Resources &res, uint gridSize,
                     const std::vector<glm::uvec3> &voxels) {
        using namespace glm;

        glBindVertexArray(VAO);

        res.voxelSP.use();
        GLint MVPLoc = glGetUniformLocation(res.voxelSP.programID, "uModelViewProjMat");
        GLint ColorLoc = glGetUniformLocation(res.voxelSP.programID, "uColor");

        const float voxelSize = 1.f / float(gridSize);

        mat4 ViewProjMat = camera.projection * camera.view;
        mat4 base = scale(mat4(1), vec3(voxelSize));

        for (size_t i = 0; i < voxels.size(); i++) {
            const auto &voxel = voxels[i];

            vec3 pos = vec3(voxel);
            mat4 model = translate(base, pos);
            mat4 MVPMat = ViewProjMat * model;

            glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, value_ptr(MVPMat));

            vec3 color3f = pos * voxelSize;
            glUniform3fv(ColorLoc, 1, value_ptr(color3f));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }
    }

    struct OctreeRenderer {
        glm::mat4 ViewProjMat;
        GLint MVPLoc;
        GLint ColorLoc;
        const Octree &octree;

        OctreeRenderer(const glm::mat4 &_viewProjMat, GLint _mvpLoc, GLint _colorLoc, const Octree &_octree) :
                ViewProjMat(_viewProjMat),
                MVPLoc(_mvpLoc),
                ColorLoc(_colorLoc),
                octree(_octree) {}

        void draw(uint32_t id, glm::uvec3 vox, float voxelSize, uint level = 0) {
            using namespace glm;

            const auto &node = octree.data[id];

            if (node.isLeaf()) {
                mat4 base = scale(mat4(1), vec3(voxelSize));
                vec3 pos(vox);

                mat4 model = translate(base, pos);
                mat4 MVPMat = ViewProjMat * model;

                glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, value_ptr(MVPMat));

//                vec3 color3f = fract(pos * voxelSize * 13643.3545f);
                vec3 color3f = vec3(node.color >> 16, (node.color >> 8) & 0xff, (node.color) & 0xff) / 255.f;
                glUniform3fv(ColorLoc, 1, value_ptr(color3f));

                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            } else {
                for (uint i = 0; i < node.children.size(); i++) {
                    if (node.children[i] != 0) {
                        draw(id + node.children[i], vox * uint(2) + VOX_OFFSET[i], voxelSize / 2.f, level + 1);
                    }
                }
            }
        }
    };

    void drawOctree(const Camera &camera, const Resources &res, const Octree &octree) const {
        if (octree.data.empty())
            return;

        using namespace glm;

        glBindVertexArray(VAO);

        res.voxelSP.use();
        GLint MVPLoc = glGetUniformLocation(res.voxelSP.programID, "uModelViewProjMat");
        GLint ColorLoc = glGetUniformLocation(res.voxelSP.programID, "uColor");

        mat4 ViewProjMat = camera.projection * camera.view;

        OctreeRenderer octreeRenderer(ViewProjMat, MVPLoc, ColorLoc, octree);
        octreeRenderer.draw(0, uvec3(0, 0, 0), 1.f);
    }

//    void drawOctree(const Camera &camera, const Resources &res, const CompactTree &octree) const {
//        if (octree.data.empty())
//            return;
//
//        using namespace glm;
//
//        glBindVertexArray(VAO);
//
//        res.voxelSP.use();
//        GLint MVPLoc = glGetUniformLocation(res.voxelSP.programID, "uModelViewProjMat");
//        GLint ColorLoc = glGetUniformLocation(res.voxelSP.programID, "uColor");
//
//        mat4 ViewProjMat = camera.projection * camera.view;
//
//        OctreeRenderer octreeRenderer(ViewProjMat, MVPLoc, ColorLoc, octree);
//        octreeRenderer.draw(0, uvec3(0, 0, 0), 1.f);
//    }
};