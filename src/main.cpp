#include "renderer/renderer.hpp"
#include "util/Status.hpp"
#include "geom/triangle.hpp"
#include "geom/voxelizer.hpp"
#include "util/Logger.hpp"

#include <iostream>
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

void testing(uint *Octree) {
    const uint MAX_LEVEL = 2;
    using namespace glm;
    //vec4 pos = gl_FragCoord;
    //float c = clamp(pow(pos.z, 30), 0, 1);
//
    uint globGridSize = 1 << MAX_LEVEL;
    uvec3 globVox = uvec3(2, 2, 2);


    const uint PARENT_BIT = 1 << 31;
    const uint PTR_MASK = ~PARENT_BIT;

    uint depth = 0;

    uvec3 curVox = uvec3(0);
    uint curPtr = PARENT_BIT;

    uint offset = 0;

    while (true) {
//        uint gridSize = 1 << (depth + 1);
//        uvec3 vox = uvec3(fPos * (gridSize - 1) + 0.5f);

        uint q = MAX_LEVEL - depth - 1;
        uvec3 vox = globVox >> q;

        uvec3 diff = vox - curVox * uint(2);

        if (diff.x > 1 || diff.y > 1 || diff.z > 1) {
            assert(false);
            return;
        }

        offset = (diff.x << 2) + (diff.y << 1) + (diff.z);

        if ((curPtr & PARENT_BIT) == 0) {
            break;
        }
        uint real_ptr = (curPtr & PTR_MASK);
        curPtr = Octree[real_ptr * 8 + offset];

        curVox = vox;

        depth++;
    }

//    vec3 color = vec3(vox) / gridSize;
    uint color = Octree[(curPtr & PTR_MASK) * 8 + offset];

    const uint RED_MASK = 0xff0000;
    const uint GREEN_MASK = 0x00ff00;
    const uint BLUE_MASK = 0x0000ff;

    uint r = color >> 16;
    uint g = (color & GREEN_MASK) >> 8;
    uint b = color & BLUE_MASK;

    vec3 colorVec = vec3(r, g, b) / 255.f;
//    vec3 colorVec = vec3(float(depth) / 2.f);
//    vec3 color = vec3() / gridSize;

    Log.info({"Color", color});
}

int main() {
    std::vector<Triangle> triangles;

    getTriangles("resources/models/cow.obj", triangles).assertOK();

    auto minAxis = glm::vec3(std::numeric_limits<float>::infinity());
    auto maxAxis = glm::vec3(-std::numeric_limits<float>::infinity());

    const float margin = 0;

    for (auto &t: triangles) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                minAxis[j] = std::min(minAxis[j], t[i][j]);
                maxAxis[j] = std::max(maxAxis[j], t[i][j]);
            }
        }
    }

    minAxis -= margin;
    maxAxis += margin;

    auto diff = maxAxis - minAxis;

    int scaleAxis = 0;
    for (int i = 1; i < 3; i++)
        if (diff[i] > diff[scaleAxis])
            scaleAxis = i;

    for (auto &t: triangles) {
        for (size_t i = 0; i < 3; i++) {
            t[i] = (t[i] - minAxis) / (diff[scaleAxis]);
        }
    }

    glm::vec3 center = (maxAxis - minAxis) / 2.f / diff[scaleAxis];

    Voxelizer::VoxelSet voxelSet = Voxelizer::voxelize(triangles);
    auto colors = Voxelizer::colorize(voxelSet);
    auto treeLevels = Voxelizer::buildLevels(voxelSet, colors);
    treeLevels.buildRaw();

//    using u32 = Voxelizer::u32;
//    u32 PBIT = Voxelizer::Octree::Node::ADDR_BIT;
//
//    for (size_t i = 0; i < treeLevels.rawData.size(); i++) {
//        printf("Node %u:\n", u32(i));
//        for (u32 v: treeLevels.rawData[i].vox) {
//            if (v & PBIT) {
//                printf("\t adr %u\n", (v ^ PBIT));
//            } else {
//                printf("\t color %#08x\n", v);
//            }
//        }
//        printf("\n");
//    }

//    testing(reinterpret_cast<uint*>(treeLevels.rawData.data()));

    {
        Renderer::RenderData data{treeLevels, triangles, center};
        Renderer r(data);
        r.mainLoop();
    }

    return 0;
}
