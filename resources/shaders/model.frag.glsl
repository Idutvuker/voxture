#version 430 core

const uint MAX_LEVEL = 9;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

layout(std430, binding = 0) buffer Voxels {
    uint Octree[];
};

vec3 sampleOctree(uvec3 targetVox) {
    const uint ADDR_BIT = 1 << 31;
    const uint PTR_MASK = ~ADDR_BIT;

    uint depth = 0;

    uvec3 curVox = uvec3(0);
    uint curPtr = ADDR_BIT;
    uint offset = 0;

    while ((curPtr & ADDR_BIT) != 0) {
        uint levelDiff = MAX_LEVEL - depth - 1;
        uvec3 vox = targetVox >> levelDiff;

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

    return vec3(r, g, b) / 255.f;
}

const uvec3 VOX_OFFSET[8] = uvec3[](
    uvec3(0, 0, 0),
    uvec3(0, 0, 1),
    uvec3(0, 1, 0),
    uvec3(0, 1, 1),

    uvec3(1, 0, 0),
    uvec3(1, 0, 1),
    uvec3(1, 1, 0),
    uvec3(1, 1, 1)
);

void main() {
    const uint gridSize = 1 << MAX_LEVEL;

    const uvec3 baseVox = uvec3(fPos * gridSize);
    const vec3 voxFract = fract(fPos * gridSize);

    vec3 colors[8];
    for (int i = 0; i < 8; i++)
        colors[i] = sampleOctree(baseVox + VOX_OFFSET[i]);

    vec3 xLerp[4];
    for (int i = 0; i < 4; i++)
        xLerp[i] = mix(colors[i], colors[i + 4], voxFract.x);

    vec3 yLerp0 = mix(xLerp[0], xLerp[2], voxFract.y);
    vec3 yLerp1 = mix(xLerp[1], xLerp[3], voxFract.y);

    vec3 color = mix(yLerp0, yLerp1, voxFract.z);

    FragColor = vec4(color, 1.0);
}
