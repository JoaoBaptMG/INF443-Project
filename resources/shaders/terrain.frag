#version 330

uniform vec3 GrassColor, SandColor, MountainColor;
uniform sampler3D NoiseTexture;
uniform float UnitsPerPeriod;

in vec4 modelPos;
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
	// Generate the color here
	vec3 color;
	if (modelPos.w < 0.75) color = MountainColor;
	else if (modelPos.y < 0) color = SandColor;
	else color = GrassColor;

	float v = texture(NoiseTexture, modelPos.xyz / UnitsPerPeriod).r;
	v = 1.4 - clamp(v * v, 0.4, 1.0);
	if (modelPos.w >= 0.75) v = 1.0;

	fragColor = computeLighting(v * color, v * color, v * Material.specularColor, Material.shininess);
}