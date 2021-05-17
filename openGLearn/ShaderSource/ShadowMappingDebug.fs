#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

//透视投影存储的阴影深度获取，需要先将深度转为线性
float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    //获取贴图颜色（深度信息，位于r通道中）
    float depthVal = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(depthVal), 1.0);
}