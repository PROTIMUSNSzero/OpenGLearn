#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    //用于变换法线、切线、副切线到世界空间的矩阵
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    //格拉姆-施密特正交化，将因为切线向量平均化而变得不再正交的的TBN向量重新正交化
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    //（TBN）为切线空间到世界空间的变换矩阵，转置TBN（同逆TBN，计算更快）为将世界空间变换到切线空间的变换矩阵
    //将所有光线变换到切线空间做计算，因为顶点着色器通常比像素着色器运行的少，顶点着色器做大部分运算开销更小
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}