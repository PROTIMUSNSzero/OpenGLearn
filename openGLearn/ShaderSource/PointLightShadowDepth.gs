#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    //�ֱ�ָ����������ͼ�ĸ�������Ϊ����棬���������������ͼԪ�Ķ��㣬
    //������任��ָ���������Ĺ�ռ�������뵽���ͼԪ���޳���������Χ��
    for(int face = 0; face < 6; face++)
    {
        //ָ����ͼ���͵���������ͼ���ĸ��������Ⱦ
        gl_Layer = face;
        for(int i = 0; i < 3; i++)
        {
            //�����Դ�ռ��µĶ���λ��
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}