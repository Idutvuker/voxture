#version 430 core

const uint MAX_LEVEL = 11;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

struct Node {
    uint children[8];
    uint color;
};

layout(std430, binding = 0) buffer Voxels {
    Node Octree[];
};

vec3 sampleOctree(uvec3 targetVox, out uint outLevel) {
    uint level = 0;

    uvec3 curVox = uvec3(0);
    uint curPtr = 0;

    while (level < MAX_LEVEL) {
        uint levelDiff = MAX_LEVEL - level - 1;
        uvec3 vox = targetVox >> levelDiff;

        uvec3 diff = vox - curVox * 2;

        uint child = (diff.x << 2) + (diff.y << 1) + (diff.z);

        uint childOffset = Octree[curPtr].children[child];

        if (childOffset == 0)
            break;

        curVox = vox;
        curPtr = curPtr + childOffset;

        level++;
    }

    uint color = Octree[curPtr].color;

    const uint RED_MASK = 0xff0000;
    const uint GREEN_MASK = 0x00ff00;
    const uint BLUE_MASK = 0x0000ff;

    uint r = (color & RED_MASK) >> 16;
    uint g = (color & GREEN_MASK) >> 8;
    uint b = color & BLUE_MASK;

    outLevel = level;
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

    uint level;

    const uvec3 baseVox = uvec3(fPos * gridSize);
    vec3 color = sampleOctree(baseVox, level);

//    const vec3 voxFract = fract(fPos * gridSize);
//
//    vec3 colors[8];
//    for (int i = 0; i < 8; i++) {
//        colors[i] = sampleOctree(baseVox + VOX_OFFSET[i], level);
//    }
//
//    vec3 xLerp[4];
//    for (int i = 0; i < 4; i++)
//        xLerp[i] = mix(colors[i], colors[i + 4], voxFract.x);
//
//    vec3 yLerp0 = mix(xLerp[0], xLerp[2], voxFract.y);
//    vec3 yLerp1 = mix(xLerp[1], xLerp[3], voxFract.y);
//
//    vec3 color = mix(yLerp0, yLerp1, voxFract.z);

    FragColor = vec4(color, 1.0);
}
