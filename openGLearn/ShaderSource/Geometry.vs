#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out V2G
{
    vec3 color;
}v2g;

void main()
{
    v2g.color = aColor;
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
}