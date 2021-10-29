#pragma once

#include "voxelizer.h"

#include <unordered_set>

namespace Renderer {
    int render(const std::unordered_set <Voxelizer::Voxel, Voxelizer::VoxelHash> &res);
}