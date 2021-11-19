#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
//    FragColor = vec4(fTexCoord, 0, 1.0);
    FragColor = texture(image, fTexCoord);
}
