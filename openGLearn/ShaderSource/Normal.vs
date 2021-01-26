#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out V2G
{
    vec3 normal;
    vec4 gl_Position;
    vec2 texCoords;
} v2g;

uniform mat4 view;
uniform mat4 model;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    v2g.normal = vec3(normalMatrix * aNormal);
    v2g.texCoords = aTexCoords;
    v2g.gl_Position = view * model * vec4(aPos, 1.0);
}