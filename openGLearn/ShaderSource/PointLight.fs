#version 460 core

out vec4 FragColor;

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//光照衰减常数
	float constant;  //常数项K0
	float linear;  //一次项K1
	float quadratic;  //二次项K2
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
	float distance = length(light.position - FragPos);
	//衰减系数 = 1 / (K0 + K1 * dis + K2 * pow(dis, 2))
	float attenuation = 1.0 / 
	(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	ambient *= attenuation;  //环境光衰减

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	diffuse *= attenuation;  //漫反射衰减

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb;
	specular *= attenuation;  //高光衰减

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}