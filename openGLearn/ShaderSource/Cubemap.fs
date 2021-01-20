#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform bool drawLighting;
uniform vec3 cameraPos;
uniform int windowWidth;

void main()
{
    if(drawLighting)
    {
        //���ߵ������嶥�����������
        vec3 I = normalize(Position - cameraPos);
        vec3 R;

        //����
        if(gl_FragCoord.x < windowWidth / 2.0)
        {
            //��������
            R = reflect(I, normalize(Normal));
        }
        //����
        else
        {
            //�����ʣ��˴�Ϊ�ӿ����������� 1����������������1.52���������ʱ�ֵ
            float ratio = 1.00 / 1.52;
            //��������
            R = refract(I, normalize(Normal), ratio);
        }

        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
}