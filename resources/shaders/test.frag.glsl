#version 430 core

in vec3 fPos;
in vec4 gl_FragCoord;

out vec4 FragColor;

void main() {
    FragColor = vec4(vec3(1.0), 1.0);
}