#version 430 core

const uint MAX_LEVEL = 7;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

layout(std430, binding = 0) buffer Voxels {
    uint Octree[];
};

void main() {
    const uint gridSize = 1 << MAX_LEVEL;
    const uvec3 globalVox = uvec3(fPos * gridSize); // TODO: FIX

    const uint ADDR_BIT = 1 << 31;
    const uint PTR_MASK = ~ADDR_BIT;

    uint depth = 0;

    uvec3 curVox = uvec3(0);
    uint curPtr = ADDR_BIT;
    uint offset = 0;

    while ((curPtr & ADDR_BIT) != 0) {
        uint levelDiff = MAX_LEVEL - depth - 1;
        uvec3 vox = globalVox >> levelDiff;

        uvec3 diff = vox - curVox * 2;

        offset = (diff.x << 2) + (diff.y << 1) + (diff.z);

        curPtr = Octree[(curPtr & PTR_MASK) * 8 + offset];

        curVox = vox;

        depth++;
    }

    uint color = curPtr;

    const uint RED_MASK = 0xff0000;
    const uint GREEN_MASK = 0x00ff00;
    const uint BLUE_MASK = 0x0000ff;

    uint r = color >> 16;
    uint g = (color & GREEN_MASK) >> 8;
    uint b = color & BLUE_MASK;

    vec3 colorVec = vec3(r, g, b) / 255.f;

    FragColor = vec4(colorVec, 1.0);
}
