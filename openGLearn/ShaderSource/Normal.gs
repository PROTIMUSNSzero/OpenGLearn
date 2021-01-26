#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in V2G
{
    vec3 normal;
    vec4 gl_Position;
    vec2 texCoords;
} v2g[];

const float MAGNITUDE = 0.2;

out vec2 TexCoords;

uniform mat4 projection;

void generateLine(int index)
{
    //法线起点
    gl_Position = projection * v2g[index].gl_Position;
    EmitVertex();
    //法线终点
    gl_Position = projection * (v2g[index].gl_Position + vec4(v2g[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generateLine(0);
    generateLine(1);
    generateLine(2);
}