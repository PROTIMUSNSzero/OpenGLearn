#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	//�ı�z��������֮���͸�ӳ��������ֵ��Ϊw/w����������ֵ����֤��պ�ͨ����ǰ��Ȳ�����˳����Ⱦ
	//��Ȳ��ԵĲ���ӦΪGL_LEQUAL
	gl_Position = pos.xyww;
}