#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//texture函数：采样纹理颜色，参数分别为纹理采样器和对应纹理坐标
	//mix函数：混合颜色，第3参数为第2颜色的插值
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}