#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

//͸��ͶӰ�洢����Ӱ��Ȼ�ȡ����Ҫ�Ƚ����תΪ����
float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    //��ȡ��ͼ��ɫ�������Ϣ��λ��rͨ���У�
    float depthVal = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(depthVal), 1.0);
}