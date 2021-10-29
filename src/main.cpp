#include "util/Status.h"
#include "triangle.h"
#include "voxelizer.h"
#include "renderer.h"

#include <iostream>
#include <unordered_map>
#include <fstream>

#include <obj_loader.h>

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

Status getTriangles(const std::string &filepath, std::vector<Triangle> &res) {
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



Status writePLY_triangles(const std::vector<Triangle> &data) {
    std::string filename = "output.ply";

    std::string header =
        "ply\n"
        "format binary_little_endian 1.0\n"
        "element vertex " + std::to_string(data.size() * 3) + "\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face " + std::to_string(data.size()) + "\n"
        "property list uint32 uint32 vertex_indices\n"
        "end_header\n";

    std::ofstream outputFile("output.ply", std::ios::out | std::ios::binary);
    if (outputFile.is_open()) {
        outputFile << header;

        outputFile.write(reinterpret_cast<const char *>(data.data()), data.size() * 3 * 3 * sizeof(float));

        uint32_t arr[4] = {3, 0, 0, 0};
        for (uint32_t i = 0; i < data.size(); i++) {
            arr[1] = i * 3;
            arr[2] = i * 3 + 1;
            arr[3] = i * 3 + 2;
            outputFile.write(reinterpret_cast<const char *>(arr), sizeof(uint32_t) * 4);
        }

        return Status::ok();
    }

    return Status::error("Couldn't open file " + filename);
}

#include <unordered_set>

int main() {
    std::vector<Triangle> triangles;

    getTriangles("models/cow.obj", triangles).assertOK();

    auto minAxis = glm::vec3(std::numeric_limits<float>::infinity());
    auto maxAxis = glm::vec3(-std::numeric_limits<float>::infinity());

    for (auto &t: triangles) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                minAxis[j] = std::min(minAxis[j], t[i][j]);
                maxAxis[j] = std::max(maxAxis[j], t[i][j]);
            }
        }
    }

    auto diff = maxAxis - minAxis;
    std::cout << glm::to_string(minAxis) << " " << glm::to_string(maxAxis) << '\n';

    int scaleAxis = 0;
    for (int i = 1; i < 3; i++)
        if (diff[i] > diff[scaleAxis])
            scaleAxis = i;

    for (auto &t: triangles) {
        for (size_t i = 0; i < 3; i++) {
            t[i] = (t[i] - minAxis) / diff[scaleAxis];
        }
    }

//    for (auto &t: triangles) {
//        std::cout << glm::to_string(t[0]) << " " << glm::to_string(t[1]) << " " << glm::to_string(t[1]) << '\n';
//    }

//    writePLY_triangles(triangles).assertOK();

    std::ofstream outputFile("mytest/voxels.txt");

    std::unordered_set<Voxelizer::Voxel, Voxelizer::VoxelHash> res;
    for (const auto &tri: triangles) {
        auto vt = Voxelizer::voxelizeTriangle(tri);
        for (const auto &v: vt) {
            res.insert(v);
        }
    }

//    for (const auto &v: res) {
//        std::cout << v.pos.x << ' ' << v.pos.z << ' ' <<  v.pos.y << '\n';
//    }

    Renderer::render(res);

    return 0;
}