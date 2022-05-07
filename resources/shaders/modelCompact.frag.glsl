#version 430 core

const uint MAX_LEVEL = 20;

in vec3 fPos;
in vec4 gl_FragCoord;

uniform sampler1D colors;

out vec4 FragColor;


struct Node {
    uint leafs;
    uint children[8];
};

layout(std430, binding = 0) buffer DAGBuffer {
    Node DAG[];
};


vec3 sampleOctree(uvec3 targetVox) {
    uint level = 0;
    uint curPtr = 0;
    uvec3 curVox = uvec3(0);

    uint rawID = 0;

    while (level < MAX_LEVEL) {
        uint levelDiff = MAX_LEVEL - level - 1;
        uvec3 vox = targetVox >> levelDiff;

        uvec3 diff = vox - curVox * 2;

        uint childNum = (diff.x << 2) + (diff.y << 1) + (diff.z);
        uint childAddr = DAG[curPtr].children[childNum];

        if (childAddr == 0)
            break;

        for (uint i = 0; i < childNum; i++) {
            uint leftChildAddr = DAG[curPtr].children[i];
            if (leftChildAddr != 0) {
                rawID += DAG[leftChildAddr].leafs;
            }
        }

        curVox = vox;
        curPtr = childAddr;

        level++;
    }

    return texelFetch(colors, int(rawID), 0);
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
    vec3 color = sampleOctree(baseVox);

    FragColor = vec4(color, 1.0);

//    vec4 c;
//    if (fPos.y < 0.5)
//        c = texelFetch(colors, 0, 0);
//    else
//        c = texelFetch(colors, 50, 0);
}
