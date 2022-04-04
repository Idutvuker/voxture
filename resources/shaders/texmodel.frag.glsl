#version 430 core

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main() {
//    FragColor = vec4(fTexCoord, 0.0, 1.0);
    FragColor = texture(texture1, fTexCoord);
}
