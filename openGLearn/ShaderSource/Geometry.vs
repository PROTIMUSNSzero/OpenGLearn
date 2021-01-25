#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

//Êä³ö½Ó¿Ú¿é
out V2G
{
    vec3 color;
    vec4 gl_Position;
}v2g;

void main()
{
    v2g.color = aColor;
    v2g.gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}