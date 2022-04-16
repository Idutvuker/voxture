#pragma once

#include <vector>
#include <string>
#include "../geom/Triangle.hpp"
#include "Status.hpp"

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
