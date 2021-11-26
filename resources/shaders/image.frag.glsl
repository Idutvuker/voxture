#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
    FragColor = vec4(vec3(texture(image, fTexCoord).r), 1.0);
}
