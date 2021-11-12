#pragma once

#include "shaderProgram.h"

struct Resources {
    ShaderProgram voxelSP = ShaderProgram("resources/shaders/voxel.vert.glsl", "resources/shaders/voxel.frag.glsl");
    ShaderProgram modelSP = ShaderProgram("resources/shaders/model.vert.glsl", "resources/shaders/model.frag.glsl");
};