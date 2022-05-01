#pragma once

#include "ShaderProgram.hpp"
#include "../data/Image.hpp"

struct Resources {
    ShaderProgram texModelSP = ShaderProgram("resources/shaders/texmodel.vert.glsl", "resources/shaders/texmodel.frag.glsl");
    ShaderProgram voxelSP = ShaderProgram("resources/shaders/voxel.vert.glsl", "resources/shaders/voxel.frag.glsl");
    ShaderProgram modelSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/modelCompact.frag.glsl");
    ShaderProgram imageSP = ShaderProgram("resources/shaders/image.vert.glsl", "resources/shaders/image.frag.glsl");
    ShaderProgram testSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/test.frag.glsl");
};
