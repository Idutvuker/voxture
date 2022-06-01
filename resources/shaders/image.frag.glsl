#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
//    vec4 c = texture(image, fTexCoord);

    ivec2 txSize = textureSize(image, 0);

    ivec2 P = ivec2(fTexCoord * txSize);
    vec4 c = texelFetch(image, P, 0);

    FragColor = c;
}
