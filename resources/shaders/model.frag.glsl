#version 430 core

const uint DATA_SIZE = 2;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

layout(std430, binding = 0) buffer Voxels {
    uint VoxelData[];
};

void main() {
    //vec4 pos = gl_FragCoord;
    //float c = clamp(pow(pos.z, 30), 0, 1);

    vec3 vox = uvec3(fPos * 255);

    if (c.x > 0.5)
        c.x = float(VoxelData[0]);
    else
        c.x = float(VoxelData[1]);

    FragColor = vec4(c, 1.0);
}
