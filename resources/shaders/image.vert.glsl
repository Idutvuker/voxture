#version 430

layout (location = 0) in vec3 aPos;

out vec2 fTexCoord;

void main() {
    fTexCoord = (aPos.xy + 1) / 2;
    fTexCoord.y = 1 - fTexCoord.y;

    gl_Position = vec4(aPos, 1.0);
}
