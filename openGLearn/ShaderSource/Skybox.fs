#version 460 core
out vec4 FragColor;

in vec3 TexCoords;
//��������ͼ������
uniform samplerCube skybox;

void main()
{
	//������������ͼ��TexCoords��Ϊ����������ָ���Ӧ�����������
	FragColor = texture(skybox, TexCoords);
}