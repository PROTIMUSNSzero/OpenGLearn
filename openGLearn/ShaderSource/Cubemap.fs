#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform int drawType;
uniform vec3 cameraPos;

void main()
{
    //����
    if(drawType == 1)
    {
        //���ߵ������嶥�����������
        vec3 I = normalize(Position - cameraPos);
        //��������
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    //����
    else if(drawType == 2)
    {
        //�����ʣ��˴�Ϊ�ӿ����������� 1����������������1.52���������ʱ�ֵ
        float ratio = 1.00 / 1.52;
        vec3 I = normalize(Position - cameraPos);
        //��������
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
}