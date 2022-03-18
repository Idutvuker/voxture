#pragma once

#include "../renderer/VoxelGrid.hpp"
#include "../renderer/DebugDraw.hpp"
#include "TreeBuilderRays.hpp"

#include "DBH.hpp"
#include "../renderer/RenderCamera.hpp"
#include "../renderer/OrbitCameraController.hpp"

struct App {
    Bundle bundle {"resources/saharov/saharov.obj", "resources/saharov/cameras.out", "resources/saharov/list.txt"};

    GLFWContext context;
    Resources res;

    VoxelGrid voxelGrid;

    RenderCamera renderCamera {float(context.WINDOW_WIDTH) / float(context.WINDOW_HEIGHT)};
    OrbitCameraController cameraController {renderCamera, context};

    struct Model {
        GLuint VAO;
        GLuint VBO;
        GLuint SSBO;

        const std::vector<Triangle> &mesh;
        const Resources &res;

        Model(const std::vector<Triangle> &_mesh, const Resources &_res) : mesh(_mesh), res(_res) {
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
            res.modelSP.use();

            GLint MVPLoc = glGetUniformLocation(res.modelSP.programID, "uModelViewProjMat");

            glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVPMat));

            glBindVertexArray(VAO);

            glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * mesh.size()));
        }
    } model {bundle.mesh, res};


    struct ViewPlane {
        GLuint VAO;
        GLuint VBO;

        const Resources &res;

        ViewPlane(const Resources &_res) : res(_res) {
            std::array<GLfloat, 6 * 3> tris = { -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 0, 1, -1, 0 };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(tris.size() * sizeof(GLfloat)), tris.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
            glEnableVertexAttribArray(0);
        }

        void draw() {
            res.imageSP.use();

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    } viewPlane { res };

    enum class DrawMode : int {
        MODEL, VOXELS, VIEW_PLANE
    } drawMode = DrawMode::MODEL;

    int maxLevelWrapper = 0;
    int DBHLevel = 0;

    DebugDraw debugDraw {};
//    std::vector<glm::vec3> points = {{0, 0, 0}, {0.2, 0.2, 0.2}, {0.3, 0.3, 0.3}, {0, 0.5, 0}};

    void draw() {
        if (drawMode == DrawMode::MODEL) {
            model.draw(renderCamera.projection * renderCamera.view);
//            model.draw(bundle.cameras.front().camera.getViewProj());
        }
        else if (drawMode == DrawMode::VOXELS) {
//            voxelGrid.drawOctree(renderCamera, res, octrees[bundleCameraID]);
//            voxelGrid.drawFromVec(renderCamera, res, (1 << treeBuilder.maxLevel), treeBuilder.voxels);
//            debugDraw.drawPoints(renderCamera, res, treeBuilder.points);
            if (!octrees.empty())
                voxelGrid.drawOctree(renderCamera, res, octrees[bundleCameraID]);
        }
        else {
            viewPlane.draw();
        }
    }

    void update(float delta) {
        cameraController.update(delta);
    }

    void run() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        loadTree();

        auto prevTime = float(glfwGetTime());
        while (!glfwWindowShouldClose(context.window)) {
            float delta = float(glfwGetTime()) - prevTime;

            glfwPollEvents();

            if (glfwGetKey(context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(context.window, true);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Begin("Controls");
                ImGui::SliderInt("orbit Rad", &GLFWContext::GLOBAL_SCROLL_Y, -5, 30);

                {
                    if (ImGui::Button("Draw Model"))
                        drawMode = DrawMode::MODEL;

                    if (ImGui::Button("Draw View Plane"))
                        drawMode = DrawMode::VIEW_PLANE;

                    if (ImGui::Button("Draw Voxels"))
                        drawMode = DrawMode::VOXELS;
                }

                if (ImGui::SliderInt("Tree max level", &maxLevelWrapper, 0, 10))
                    treeBuilder.maxLevel = uint(maxLevelWrapper);

                if (ImGui::SliderInt("DBH level", &DBHLevel, 0, int(dbh.data.size() - 1)))
                    dbh.debugLevel(DBHLevel);

//                ImGui::Text("Voxel count: %zu", treeBuilder.octree.data.size());

                ImGui::SliderInt("Bundle Camera", &bundleCameraID, 0, int(bundle.cameras.size() - 1));

                if (ImGui::Button("Rebuild all trees"))
                    fullBuild();

                if (ImGui::Button("Load tree"))
                    loadTree();

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

    App() = default;

    DBH dbh {context, bundle.cameras.front().photo->width, bundle.cameras.front().photo->height};
    TreeBuilderRays treeBuilder {bundle.mesh};

    std::function<void(const glm::mat4&)> drawFunc = [this] (const glm::mat4& MVPMat) { model.draw(MVPMat); };

    int bundleCameraID = 0;
    std::vector<Octree> octrees;

    void fullBuild() {
        for (size_t i = 0; i < bundle.cameras.size(); i++) {
            printf("\rBuilding octrees %zu/%zu", i + 1, bundle.cameras.size());
            fflush(stdout);

            const auto &cam = bundle.cameras[i];
            auto MVP = cam.camera.getViewProj();
            float focalLength = cam.focalLength;

            auto depthMap = dbh.calcDepthMap(drawFunc, MVP);
            const auto &photo = bundle.cameras[i].photo.value();

            auto octree = treeBuilder.buildTree(MVP, focalLength, depthMap, photo);
            DiskTree::save(octree, "out/" + std::to_string(i) + ".tree");
//            DiskKeys::Saver(octree, "out5/" + std::to_string(i) + ".keys").save();
        }

        printf("\rBuilding octrees done!\n");
    }

    void loadTree() {
        octrees.clear();
//        Octree octree{};
//        octree.data.emplace_back();
//        octrees.push_back(octree);
        octrees.emplace_back("out2/join_9.tree");
//
        model.updateTree(octrees.front());
    }
};