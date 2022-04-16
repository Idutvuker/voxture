#version 430 core

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

const float CAMERA_NEAR = 0.005f;
const float CAMERA_FAR = 10.f;

float linDepth(float ndc) {
    return (2 * CAMERA_NEAR * CAMERA_FAR) / (CAMERA_FAR + CAMERA_NEAR - ndc * (CAMERA_FAR - CAMERA_NEAR));;
}

void main() {
    float ndc = gl_FragCoord.z * 2 - 1;
    float ld = linDepth(ndc) / 1.5f;

    FragColor = vec4(vec3(ld), 1.0);
}