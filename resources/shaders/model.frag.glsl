#version 430 core

const uint MAX_LEVEL = 2;

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

uniform uint uLevel = 0;

layout(std430, binding = 0) buffer Voxels {
    uint Octree[];
};

void main() {
    //vec4 pos = gl_FragCoord;
    //float c = clamp(pow(pos.z, 30), 0, 1);
//
    uint globGridSize = 1 << MAX_LEVEL;
    uvec3 globVox = uvec3(fPos * (globGridSize - 1) + 0.5f);


    const uint PARENT_BIT = 1 << 31;
    const uint PTR_MASK = ~PARENT_BIT;

    uint depth = 0;

    uvec3 curVox = uvec3(0);
    uint curPtr = PARENT_BIT;

    uint offset = 0;

    while (true) {
        uint gridSize = 1 << (depth + 1);
//        uvec3 vox = uvec3(fPos * (gridSize - 1) + 0.5f);

        uint q = MAX_LEVEL - depth - 1;
        uvec3 vox = uvec3(globVox.x >> q, globVox.y >> q, globVox.z >> q);
//        uvec3 ox

        uvec3 nextVox = curVox * 2;

        ivec3 diff = ivec3(vox) - ivec3(nextVox);

        if (diff.x > 1 || diff.y > 1 || diff.z > 1) {
            FragColor = vec4(1, 1, 0, 1.0);
            return;
        }

        offset = (diff.x << 2) + (diff.y << 1) + (diff.z);

        if ((curPtr & PARENT_BIT) == 0) {
            break;
        }

        curPtr = Octree[(curPtr & PTR_MASK) * 8 + offset];

        curVox = nextVox;

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

    FragColor = vec4(colorVec, 1.0);
}

//void main() {
//    uint gridSize = 1 << MAX_LEVEL;
//    uvec3 globVox = uvec3(fPos * (gridSize - 1) + 0.5f);
//
//    FragColor = vec4(colorVec, 1.0);
//}
