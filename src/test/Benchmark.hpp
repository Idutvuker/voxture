#pragma once

#include <vector>
#include "../renderer/Model.hpp"

struct Benchmark {
    static constexpr size_t FRAMES_PER_VIEW = 60;
    double stamp = 0;

    std::vector<double> history;

    const GLFWContext &context;
    RenderCamera renderCam{float(context.windowWidth) / float(context.windowHeight)};

    explicit Benchmark(const GLFWContext &_context) :
            context(_context) {}

    double measure() {
        double newStamp = glfwGetTime();
        double elapsed = newStamp - stamp;

        stamp = newStamp;

        return elapsed;
    }

    double testView(const Drawable &model, const glm::mat4 &MVP) {
        std::vector<double> data;

        measure();

        for (size_t frame = 0; frame < FRAMES_PER_VIEW; frame++) {
            glfwPollEvents();

            glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model.draw(MVP);

            glfwSwapBuffers(context.window);
        }

        auto elapsed = measure();

        return elapsed / FRAMES_PER_VIEW;
    }

    static std::vector<glm::mat4> getCameras() {
        std::ifstream input(BENCHMARK_CAMERAS_PATH);

        std::vector<float> data;
        float next;

        while (input >> next)
            data.push_back(next);

        std::vector<glm::mat4> result;

        for (int k = 0; k < data.size(); k += 16) {
            glm::mat4 mat;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    mat[i][j] = data[k + i * 4 + j];
                }
            }

            result.push_back(mat);
        }

        return result;
    }

    void start(const Drawable &model) {
        auto cameras = getCameras();

        for (const auto &cam: cameras) {
            history.push_back(testView(model, cam));
        }

        saveResultsToDisk();
    }

    void saveResultsToDisk() {
        std::ofstream output("test/results.csv");

        for (const auto &x: history) {
            output << x << ',';
        }
    }
};
