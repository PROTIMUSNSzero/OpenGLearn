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
    //反射
    if(drawType == 1)
    {
        //视线到立方体顶点的入射向量
        vec3 I = normalize(Position - cameraPos);
        //反射向量
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    //折射
    else if(drawType == 2)
    {
        //折射率，此处为从空气（折射率 1）到玻璃（折射率1.52）的折射率比值
        float ratio = 1.00 / 1.52;
        vec3 I = normalize(Position - cameraPos);
        //折射向量
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
}