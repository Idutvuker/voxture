#pragma once

#include "ShaderProgram.hpp"
#include "../common/Image.hpp"

struct Resources {
//    ShaderProgram pointSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/point.frag.glsl");
    ShaderProgram voxelSP = ShaderProgram("resources/shaders/voxel.vert.glsl", "resources/shaders/voxel.frag.glsl");
    ShaderProgram modelSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/modelCompact.frag.glsl");
    ShaderProgram imageSP = ShaderProgram("resources/shaders/image.vert.glsl", "resources/shaders/image.frag.glsl");
    ShaderProgram testSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/test.frag.glsl");
};
