#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int drawType;

const float offset = 1.0 / 300.0;

void main()
{
    //反向
    if(drawType == 1)
    {
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    }
    //灰度
    else if(drawType == 2)
    {
        FragColor = texture(screenTexture, TexCoords);
        //float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
        //加权通道
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else if(drawType >= 3 && drawType <= 5)
    {
        vec2 offset[9] = vec2[]
        (
            vec2(-offset,  offset),
            vec2( 0.0f,    offset),
            vec2( offset,  offset),
            vec2(-offset,  0.0f),
            vec2( 0.0f,    0.0f),
            vec2( offset,  0.0f),
            vec2(-offset, -offset),
            vec2( 0.0f,   -offset),
            vec2( offset, -offset)
            //数组最后一项不能加','
        );
        float kernel[9];
        if(drawType == 3)
        {
            //锐化
            kernel = float[]
            (
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );
        }
        else if(drawType == 4)
        {
            //模糊
            kernel = float[]
            (
                1.0 / 16, 2.0 / 16, 1.0 / 16,
                2.0 / 16, 4.0 / 16, 2.0 / 16,
                1.0 / 16, 2.0 / 16, 1.0 / 16
            );
        }
        else if(drawType == 5)
        {
            //边缘检测
            kernel = float[]
            (
                1,  1, 1,
                1, -8, 1,
                1,  1, 1
            );
        }

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offset[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
        {
            col += sampleTex[i] * kernel[i];
        }

        FragColor = vec4(col, 1.0);
    }
    else
    {
        vec3 color = texture(screenTexture, TexCoords).rgb;
        FragColor = vec4(color, 1.0);
    }



}