#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
    float c = texture(image, vec2(fTexCoord.x, 1 - fTexCoord.y)).r;
//    if (c == 1.f)
//        c = 0.f;

    FragColor = vec4(vec3(c), 1.0);
}
