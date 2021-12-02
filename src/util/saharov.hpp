#pragma once

#include "../geom/triangle.hpp"

struct SaharovLoader {
    std::vector<Triangle> triangles;

    void load() {
        getTriangles("resources/saharov/saharov.obj", triangles).assertOK();
    }
};