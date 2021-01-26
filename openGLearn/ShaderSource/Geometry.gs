#version 460 core

//布局修饰符 指定输入图元类型（对应于glDrawArray中的图元类型）
//points; lines; lines_adjacency; triangles; triangles_adjacency;
layout (points) in;
//指定输出的图元类型 和 最大输出顶点数量
//points; line_strip; triangle_strip
layout (triangle_strip, max_vertices = 5) out;

//输入接口块，同顶点shader的输出接口块。此处为1个输入图元的所有数据，即1个顶点。
in V2G
{
    vec3 color;
    vec4 gl_Position;
}v2g[];

out vec3 fColor;

void buildHouse(vec4 position)
{
    fColor = v2g[0].color;
    gl_Position = position + vec4(-0.2, -0.2,  0.0,  0.0);
    EmitVertex();  //调用后将gl_Position的向量添加到图元中
    //triangle_strip图元输出类型会将添加的相邻3顶点连接成三角形;line_strip输出类型会将添加的相邻2顶点连接成线条
    gl_Position = position + vec4( 0.2, -0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4,  0.0,  0.0);
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();  //调用后将之前添加（Emit）的所有顶点生成为指定的输出渲染图元，重复调用可重复生成图元
}

void main()
{
    //以顶点shader输入的1个顶点为基础绘制新三角图形
    buildHouse(v2g[0].gl_Position);
}