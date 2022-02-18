#version 330 core

in vec4 gl_FragCoord;

out vec4 FragColor;

void main() {
    vec4 pos = gl_FragCoord;
    float c = clamp(pow(pos.z, 30), 0, 1);

    FragColor = vec4(1.0);
}
