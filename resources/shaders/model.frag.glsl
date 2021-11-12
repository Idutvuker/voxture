#version 430 core

const uint MAX_LEVEL = 6;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

uniform uint uLevel = 0;

//layout(std430, binding = 0) buffer Voxels {
//    uint VoxelData[];
//};

void main() {
    //vec4 pos = gl_FragCoord;
    //float c = clamp(pow(pos.z, 30), 0, 1);

    uint gridSize = 1 << uLevel;

    uvec3 vox = uvec3(fPos * (gridSize - 1) + 0.5f);

    vec3 color = vec3(vox) / gridSize;

    FragColor = vec4(color, 1.0);
}
