#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
	//顶点位置与变换矩阵相乘
	gl_Position = transform * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}