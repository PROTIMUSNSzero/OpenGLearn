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
        //视线到立方体顶点的入射向量
        vec3 I = normalize(Position - cameraPos);
        vec3 R;

        //反射
        if(gl_FragCoord.x < windowWidth / 2.0)
        {
            //反射向量
            R = reflect(I, normalize(Normal));
        }
        //折射
        else
        {
            //折射率，此处为从空气（折射率 1）到玻璃（折射率1.52）的折射率比值
            float ratio = 1.00 / 1.52;
            //折射向量
            R = refract(I, normalize(Normal), ratio);
        }

        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
}