#version 460 core

//�󶨵����ã�ͬglUniformBlockBinding��
layout (std140, binding = 1) uniform Alpha
{
    float alpha;
};
out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = vec4(vec3(color.xyz), alpha);
}