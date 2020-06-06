#version 330

in vec4 color;

out vec4 fragColor;

struct MaterialDefinition
{
	vec3 specularColor;
	float shininess;
};

uniform MaterialDefinition Material;

vec4 computeLighting(vec3 ambient, vec3 diffuse, vec3 specular, float shininess);

void main()
{
	fragColor = computeLighting(color.xyz, color.xyz, Material.specularColor, Material.shininess);
}