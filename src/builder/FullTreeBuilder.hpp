#pragma once

#include "../common/constants.hpp"
#include "../renderer/GLFWContext.hpp"
#include "../bundle/Bundle.hpp"
#include "../renderer/Resources.hpp"
#include "PartialTreeBuilder.hpp"
#include "../data/RawOctree.hpp"
#include "../renderer/Model.hpp"
#include "DiskTree.hpp"
#include "CompactOctreeBuilder.hpp"

#include <functional>
#include <filesystem>

#include "stb_image_write.h"

namespace fs = std::filesystem;

struct FullTreeBuilder {
    Bundle<> bundle;
    std::string outputPath;

    GLFWContext context {100, 100, false};
    Resources res;

    OctreeTexModel model{bundle.mesh, res.testSP};

    PartialTreeBuilder treeBuilder;
    std::function<void(const glm::mat4&)> drawFunc = [this] (const glm::mat4& MVPMat) { model.draw(MVPMat); };

    fs::path buildRec(uint left, uint right) {
        if (right - left < 1)
            return {};

        if (right - left == 1)
            return buildTree(left);

        uint mid = (left + right) / 2;
        auto leftTree = buildRec(left, mid);
        auto rightTree = buildRec(mid, right);

        if (leftTree.empty())
            return rightTree;
        if (rightTree.empty())
            return leftTree;

        fs::path newPath = outputPath + (std::to_string(left) + '_' + std::to_string(right)) + ".tree";

        DiskTree::merge(leftTree.string(), rightTree.string(), newPath.string());

        fs::remove(leftTree);
        fs::remove(rightTree);

        std::cout << "Done: " << left << ' ' << right << std::endl;

        return newPath;
    }

    fs::path buildFull(bool buildCompact = false) {
        auto res = buildRec(0, bundle.cameras.size());

        if (buildCompact) {
            std::cout << "Building compact octree" << std::endl;
            CompactOctreeBuilder::build(outputPath + "compact");
        }

        return res;
    }

    fs::path buildTree(uint cameraId) {
        const auto &cam = bundle.cameras[cameraId];
        auto MVP = cam.camera.getViewProj();
        float focalLength = cam.focalLength;

        DepthReader depthReader(cam.photoInfo.dims);
        auto depthMap = depthReader.readDepthMap(drawFunc, MVP);

        std::cout << "Building tree " << cameraId << std::endl;

        try {
            auto photo = cam.photoInfo.loadImage();
            auto octree = treeBuilder.buildTree(MVP, focalLength, depthMap, photo);

            auto newPath = fs::path(outputPath + std::to_string(cameraId) + ".tree");
            DiskTree::save(octree, newPath.string());

            return newPath;
        } catch (const std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
            return {};
        }
    }

    struct DepthReader {
        GLuint depthTex = 0;
        GLuint depthFBO = 0;

        glm::uvec2 dims;

        DepthReader(const glm::uvec2 &_dims) : dims(_dims) {
            glGenTextures(1, &depthTex);
            glBindTexture(GL_TEXTURE_2D, depthTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GLsizei(dims.x), GLsizei(dims.y), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenFramebuffers(1, &depthFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        ~DepthReader() {
            glDeleteFramebuffers(1, &depthFBO);
            glDeleteTextures(1, &depthTex);
        }

        Image<float> readDepthMap(const std::function<void(const glm::mat4&)> &drawFunc, const glm::mat4 &MVPMat) const {
            glViewport(0, 0, GLsizei(dims.x), GLsizei(dims.y));
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            drawFunc(MVPMat);

            Image<float> depthMap(dims.x, dims.y);

            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthMap.image.data());

            return depthMap;
        }
    };

    explicit FullTreeBuilder(const std::string &bundlePath, const std::string &_outputPath) :
        bundle(bundlePath + "model.ply", bundlePath + "cameras.out", bundlePath + "list.txt"),
        outputPath(_outputPath)
        {}
};


