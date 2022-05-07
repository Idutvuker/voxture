#pragma once

#include "GLFWContext.hpp"
#include "Resources.hpp"
#include "RenderCamera.hpp"
#include "OrbitCameraController.hpp"

struct ModelViewer {
    GLFWContext context;
    Resources res;

    RenderCamera renderCamera{float(context.windowWidth) / float(context.windowHeight)};
    OrbitCameraController cameraController{renderCamera, context};

    virtual void run() = 0 ;

    virtual void runBenchmark() = 0;
    virtual ~ModelViewer() = default;
};