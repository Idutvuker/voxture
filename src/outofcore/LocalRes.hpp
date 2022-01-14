#pragma once

#include <glm/glm.hpp>
#include "../geom/Triangle.hpp"

struct LocalRes {
    std::vector<Triangle> mesh;
    std::vector<BundleCamera> cameras;

    LocalRes() {
        getTriangles("resources/models/tri.obj", mesh);


        glm::mat4 projection = glm::perspective(70.f, 2.f, CAMERA_NEAR, CAMERA_FAR);

        Image<glm::u8vec3> img1("resources/textures/bigTest.jpg");
        Image<glm::u8vec3> img2("resources/textures/test.jpg");

    }
};