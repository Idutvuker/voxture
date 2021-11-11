#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModelViewProjMat;

out vec3 fPos;

void main()
{
    fPos = aPos;
    gl_Position = uModelViewProjMat * vec4(aPos, 1.0f);
}
