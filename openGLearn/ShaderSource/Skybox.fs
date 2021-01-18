#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	//采样立方体贴图（TexCoords亦为立方体中心指向对应顶点的向量）
	FragColor = texture(skybox, TexCoords);
}