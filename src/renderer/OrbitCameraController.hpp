#pragma once

#include "mygl.hpp"

class OrbitCameraController {
    RenderCamera &camera;
    GLFWContext &context;

    const float rotSens = 0.01f;
    const float moveSens = 0.001f;

    glm::vec2 rot{};
    glm::mat4 rotMatrix{1};
    glm::vec3 orbitBase{};

    bool mouseWasPressed = false;
    glm::vec2 lastMousePos{};
public:
    float orbitRadius = 2.5;

    OrbitCameraController(RenderCamera &cameraRef, GLFWContext &contextRef) : camera(cameraRef), context(contextRef) {
        GLFWContext::GLOBAL_SCROLL_Y = 20;
    }

    void update(float) {
        using namespace glm;

        bool LMBPressed = glfwGetMouseButton(context.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        bool RMBPressed = glfwGetMouseButton(context.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;

        if (LMBPressed || RMBPressed) {
            double xpos, ypos;
            glfwGetCursorPos(context.window, &xpos, &ypos);

            vec2 curMousePos = vec2(xpos, ypos);

            if (mouseWasPressed) {
                vec2 delta = curMousePos - lastMousePos;
                if (LMBPressed) {
                    rot += vec2(delta.y, delta.x) * rotSens;
                    rot.x = clamp(rot.x, -half_pi<float>(), half_pi<float>());

                    rotMatrix = glm::rotate(mat4(1), rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                glm::rotate(mat4(1), rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

                } else {
                    vec4 moveDelta = vec4(delta.x * moveSens, -delta.y * moveSens, 0, 1);
                    orbitBase += vec3(moveDelta * rotMatrix);
                }
            }

            lastMousePos = curMousePos;
            mouseWasPressed = true;
        } else {
            mouseWasPressed = false;
        }

        orbitRadius = 10.f / (1.f + expf(float(GLFWContext::GLOBAL_SCROLL_Y) / 10.f));

        camera.view =
                translate(mat4(1), vec3(0, 0, -orbitRadius)) *
                rotMatrix *
                translate(mat4(1), orbitBase);
    }
};