#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uModelViewProjMat;

out vec2 fTexCoord;

void main()
{
    gl_Position = uModelViewProjMat * vec4(aPos, 1.0f);
    fTexCoord = vec2(aTexCoord.x, -aTexCoord.y);
}
