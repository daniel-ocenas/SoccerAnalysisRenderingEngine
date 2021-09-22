#version 460 core
layout (location = 0) in vec3 pos;

uniform mat4 lightSpaceMat;
uniform mat4 modelMat;

void main()
{
    gl_Position = lightSpaceMat * modelMat * vec4(pos, 1.0);
}