#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; face++)
    {
        //指定将图形送到立方体贴图的哪个面
        gl_Layer = face;
        for(int i = 0; i < 3; i++)
        {
            //计算光源空间下的顶点位置
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}