#pragma once

#include "common/constants.hpp"
#include "../renderer/GLFWContext.hpp"
#include "../bundle/Bundle.hpp"
#include "renderer/Resources.hpp"
#include "outofcore/TreeBuilderRays.hpp"
#include "outofcore/Octree.hpp"
#include "outofcore/Model.hpp"
#include "outofcore/DBH.hpp"

#include <functional>

#include "stb_image_write.h"

struct DiskTreeBuilder {
    Bundle bundle {"resources/saharov/saharov.obj", "resources/saharov/cameras.out", "resources/saharov/list.txt"};

    GLFWContext context {100, 100, false};
    Resources res;

    Model model{bundle.mesh, res.testSP};

    TreeBuilderRays treeBuilder;
    std::function<void(const glm::mat4&)> drawFunc = [this] (const glm::mat4& MVPMat) { model.draw(MVPMat); };

    DiskTreeBuilder() = default;

    void buildAllTrees() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        for (size_t i = 0; i < bundle.cameras.size(); i++) {
            printf("\rBuilding octrees %zu/%zu", i + 1, bundle.cameras.size());
            fflush(stdout);

            const auto &cam = bundle.cameras[i];
            auto MVP = cam.camera.getViewProj();
            float focalLength = cam.focalLength;

            DepthReader depthReader({cam.photo->width, cam.photo->height});

            auto depthMap = depthReader.calcDepthMap(drawFunc, MVP);

            /*{
                Image<glm::u8vec3> test(depthMap.width, depthMap.height);

                for (int j = 0; j < test.width * test.height; j++) {
                    test.image[j] = glm::u8vec3(depthMap.image[j] == 1.f ? 0 : 255);
                }

                stbi_write_bmp("out/test.bmp", test.width, test.height, 3, test.image.data());
            }*/

            const auto &photo = bundle.cameras[i].photo.value();

            auto octree = treeBuilder.buildTree(MVP, focalLength, depthMap, photo);
            DiskTree::save(octree, "out/" + std::to_string(i) + ".tree");
        }

        printf("\rBuilding octrees done!\n");
    }

    struct DepthReader {
        GLuint depthTex = 0;
        GLuint depthFBO = 0;

        glm::ivec2 dims;

        DepthReader(const glm::ivec2 &_dims) : dims(_dims) {
            glGenTextures(1, &depthTex);
            glBindTexture(GL_TEXTURE_2D, depthTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dims.x, dims.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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

        Image<float> calcDepthMap(const std::function<void(const glm::mat4&)> &drawFunc, const glm::mat4 &MVPMat) {
            glViewport(0, 0, dims.x, dims.y);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            drawFunc(MVPMat);

            Image<float> depthMap(dims.x, dims.y);

            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthMap.image.data());

            return depthMap;
        }
    };
};


