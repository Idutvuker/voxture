#pragma once

#include <vector>
#include "../renderer/Model.hpp"

struct Benchmark {
    static constexpr size_t FRAMES = 60 * 10;
    double stamp = 0;

    std::vector<double> history;

    const GLFWContext &context;
    RenderCamera camera = RenderCamera {float(context.windowWidth) / float(context.windowHeight)};
    explicit Benchmark(const GLFWContext &_context) : context(_context) {}

    void measure() {
        double newStamp = glfwGetTime();
        double elapsed = newStamp - stamp;

        stamp = newStamp;

        history.push_back(elapsed);
    }

    glm::mat4 getCameraPos(size_t frame) {
        using namespace glm;

        float alpha = float(frame) / float(FRAMES - 1);

        camera.view = glm::rotate(translate(mat4(1), vec3(0, 0, -2)), alpha * 2.f * pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));

        return camera.getViewProj();
    }

    void start(const Drawable &model) {
        stamp = glfwGetTime();

        for (size_t frame = 0; frame < FRAMES; frame++) {
            glfwPollEvents();
            measure();

            glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model.draw(getCameraPos(frame));

            glfwSwapBuffers(context.window);
        }
    }

    void analyze() {
        double sum = 0;

        for (const auto &x: history) {
            sum += x;
        }

        double avgTime = sum / double(history.size());

        std::cout << "Avg fps: " << 1.0 / avgTime << std::endl;
    }
};
