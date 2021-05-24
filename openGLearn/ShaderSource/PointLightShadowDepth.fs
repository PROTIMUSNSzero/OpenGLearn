#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    //������Ȳ�ӳ����[0��1]֮��
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / far_plane;
    gl_FragDepth = lightDistance;
}