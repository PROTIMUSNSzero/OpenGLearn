#version 460 core

out vec4 FragColor;

struct Light
{
	vec3 position;
	//�۹��ǰ������
	vec3 direction;
	//�й��(�й�����ڹ���ǿ��Ϊ1)
	float cutOff;
	//���й�ǣ����й�������й�ǹ�����1˥����0��
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	//��theta��ֵ���޶���0 - 1֮��
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	diffuse *= intensity;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb;
	specular *= intensity;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}