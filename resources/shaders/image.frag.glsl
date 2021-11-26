#version 430

in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D image;

const float near = 0.1f;
const float far = 100.0f;

void main() {
    FragColor = vec4(fTexCoord, 0, 1.0);
    float depth = texture(image, fTexCoord).r;
//    float ndc = depth * 2.0 - 1.0;
//    float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
//
    FragColor = vec4(vec3(depth), 1);
}
