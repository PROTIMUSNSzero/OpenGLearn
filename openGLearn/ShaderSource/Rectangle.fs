#version 460 core
out vec4 FragColor;
uniform vec4 ourColor;  //uniform全局变量

void main()
{
	FragColor = ourColor;
}