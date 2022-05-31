#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

void main() {
//    vec4 c = texture(image, fTexCoord);

    ivec2 txSize = textureSize(image, 0);

    ivec2 P = ivec2(fTexCoord * txSize);
    ivec2 P2 = ivec2(P.y * 256 + P.x, 0);
    vec4 c = texelFetch(image, P, 0);

    FragColor = c;
}
