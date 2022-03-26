#version 430 core

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

const float CAMERA_NEAR = 0.01f;
const float CAMERA_FAR = 50.f;

float linDepth(float ndc) {
    return (2 * CAMERA_NEAR * CAMERA_FAR) / (CAMERA_FAR + CAMERA_NEAR - ndc * (CAMERA_FAR - CAMERA_NEAR));;
}

void main() {
    FragColor = vec4(fPos, 1.0);
}