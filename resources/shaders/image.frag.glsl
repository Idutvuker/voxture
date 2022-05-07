#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
    vec4 c = texture(image, vec2(fTexCoord.x, 1 - fTexCoord.y));

    FragColor = c;
}
