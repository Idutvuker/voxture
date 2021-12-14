#version 430

layout (location = 0) in vec3 aPos;

out vec2 fTexCoord;

void main() {
    fTexCoord = (vec2(aPos.x, -aPos.y) + 1) / 2;

    gl_Position = vec4(aPos, 1.0);
}
