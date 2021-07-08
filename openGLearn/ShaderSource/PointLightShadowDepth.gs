#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    //分别指定立方体贴图的各个面作为输出面，遍历输入的三角形图元的顶点，
    //将顶点变换到指定面所属的光空间里，最后加入到输出图元或剔除（超出范围）
    for(int face = 0; face < 6; face++)
    {
        //指定将图形送到立方体贴图的哪个面进行渲染
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