#version 460 core
out vec4 Fragcolor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	//������������ͼ��TexCoords��Ϊ����������ָ���Ӧ�����������
	FragColor = texture(skybox, TexCoords);
}