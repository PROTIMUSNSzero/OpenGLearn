#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	//改变z分量，在之后的透视除法中深度值变为w/w，即最大深度值，保证天空盒通过提前深度测试以顺利渲染
	//深度测试的参数应为GL_LEQUAL
	gl_Position = pos.xyww;
}