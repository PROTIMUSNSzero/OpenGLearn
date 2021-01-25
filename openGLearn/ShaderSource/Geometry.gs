#version 460 core

//�������η� ָ������ͼԪ���ͣ���Ӧ��glDrawArray�е�ͼԪ���ͣ�
//points; lines; lines_adjacency; triangles; triangles_adjacency;
layout (points) in;
//ָ�������ͼԪ���� �� ��������������
//points; line_strip; triangle_strip
layout (triangle_strip, max_vertices = 5) out;

//����ӿڿ飬ͬ����shader������ӿڿ顣�˴�Ϊ1������ͼԪ�����ж��㡣points����ͼԪ����ֻ����1������
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
    EmitVertex();  //���ú�gl_Position��������ӵ�ͼԪ��
    //triangle_stripͼԪ������ͻὫ��ӵ�����3�������ӳ�������;line_strip������ͻὫ��ӵ�����2�������ӳ�����
    gl_Position = position + vec4( 0.2, -0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2,  0.0,  0.0);
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4,  0.0,  0.0);
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();  //���ú�֮ǰ��ӣ�Emit�������ж�������Ϊָ���������ȾͼԪ���ظ����ÿ��ظ�����ͼԪ
}

void main()
{
    //�Զ���shader�����1������Ϊ��������������ͼ��
    buildHouse(v2g[0].gl_Position);
}