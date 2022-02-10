#pragma once

#include "../renderer/VoxelGrid.hpp"
#include "TreeBuilder.hpp"

#include "DBH.hpp"

struct App {
    Bundle bundle {"resources/models/teddy.obj", "resources/testBundle/cameras.out", "resources/testBundle/list.txt"};
//    Bundle bundle {"resources/saharov/saharov.obj", "resources/saharov/cameras.out", "resources/saharov/list.txt"};

    GLFWContext context;
    Resources res;

    VoxelGrid voxelGrid;

    RenderCamera renderCamera {float(context.WINDOW_WIDTH) / float(context.WINDOW_HEIGHT)};
    OrbitCameraController cameraController {renderCamera, context};

    struct Model {
        GLuint VAO;
        GLuint VBO;

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
        }

        void draw(const glm::mat4 &MVPMat) const {
            using namespace glm;
            res.testSP.use();

            GLint MVPLoc = glGetUniformLocation(res.testSP.programID, "uModelViewProjMat");

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

    void draw() {
        if (drawMode == DrawMode::MODEL) {
            model.draw(renderCamera.projection * renderCamera.view);
        }
        else if (drawMode == DrawMode::VOXELS) {
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

                ImGui::Text("Voxel count: %zu", treeBuilder.octree.data.size());

                ImGui::SliderInt("Bundle Camera", &bundleCameraID, 0, int(bundle.cameras.size() - 1));

                if (ImGui::Button("Rebuild all trees"))
                    fullBuild();

                if (ImGui::Button("Save trees"))
                    saveTrees();

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
    TreeBuilder treeBuilder {bundle.mesh, dbh};

    std::function<void(const glm::mat4&)> drawFunc = [this] (const glm::mat4& MVPMat) { model.draw(MVPMat); };

    int bundleCameraID = 0;

    void rebuildTree() {
        const BundleCamera &cam = bundle.cameras[bundleCameraID];
        glm::mat4 MVP = cam.camera.getViewProj();

        dbh.update(drawFunc, MVP);
        treeBuilder.buildTree(MVP, cam.photo.value());
    }


    std::vector<Octree> octrees;

    void fullBuild() {
        octrees.clear();

        for (size_t i = 0; i < bundle.cameras.size(); i++) {
            printf("Building. %zu/%zu\n", i + 1, bundle.cameras.size());

            const auto &cam = bundle.cameras[i];
            glm::mat4 MVP = cam.camera.getViewProj();

            dbh.update(drawFunc, MVP);
            treeBuilder.buildTree(MVP, cam.photo.value());

            octrees.push_back(treeBuilder.octree);
        }
    }

    void saveTrees() {
        octrees.front().saveToDisk("2.tree");
//        for (size_t i = 0; i < octrees.size(); i++) {
//            octrees[i].saveToDisk("data_" + std::to_string(i) + ".tree");
//        }
    }

    void loadTree() {
        octrees.emplace_back("2.tree");
    }
};