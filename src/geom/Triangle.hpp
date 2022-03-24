#pragma once

#include "../util/Status.hpp"

#include <iostream>
#include <glm/vec3.hpp>
#include <vector>
#include <obj_loader.h>

#include "happly.h"

struct Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    glm::vec3 &operator[](size_t index) {
        if (index == 0)
            return a;
        if (index == 1)
            return b;
        return c;
    }

    const glm::vec3 &operator[](size_t index) const {
        if (index == 0)
            return a;
        if (index == 1)
            return b;
        return c;
    }
};


inline Status getTriangles(const std::string &filepath, std::vector<Triangle> &res) {
    happly::PLYData plyData(filepath);

    std::vector<float> vPosX = plyData.getElement("vertex").getProperty<float>("x");
    std::vector<float> vPosY = plyData.getElement("vertex").getProperty<float>("y");
    std::vector<float> vPosZ = plyData.getElement("vertex").getProperty<float>("z");

    std::vector<std::vector<size_t>> fInd = plyData.getFaceIndices<size_t>();

    for (const auto &tri: fInd) {
        assert(tri.size() == 3);

        Triangle newTri;
        for (int i = 0; i < 3; i++) {
            size_t idx = tri[i];
            newTri[i] = glm::vec3(vPosX[idx], vPosY[idx], vPosZ[idx]);
        }

        res.push_back(newTri);
    }

    return Status::ok();
}


inline Status getTrianglesFromOBJ(const std::string &filepath, std::vector<Triangle> &res) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return Status::error(reader.Error());
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();

    for (const auto &shape: shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            auto num_vertices = size_t(shape.mesh.num_face_vertices[f]);

            assert(num_vertices == 3 && "Face is not a triangle");

            Triangle t{};

            for (size_t v = 0; v < num_vertices; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                t[v] = {
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2]
                };
            }

            res.push_back(t);

            index_offset += num_vertices;
        }
    }

    return Status::ok();
}