#version 330

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform float Shininess;

in vec2 texcoord;

out vec4 fragColor;

vec4 computeLighting(vec3 ambient, vec3 diffuse, vec3 specular, float shininess);

void main()
{
	vec4 diffuse = texture(DiffuseTexture, texcoord);

	fragColor = computeLighting(diffuse.xyz, diffuse.xyz, vec3(0.0, 0.0, 0.0), Shininess);
}
