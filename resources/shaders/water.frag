#version 330

uniform sampler2D ReflectionTexture;
uniform sampler2D RefractionTexture;
uniform mat4 View;
uniform vec3 ViewNormal;
uniform float WaveHeight;

struct DirectionalLight
{
	vec3 directionView;
	vec3 ambient, diffuse, specular;
};

uniform DirectionalLight Light;

uniform sampler2D RippleTextures[2];
uniform vec2 Offsets[2];
uniform sampler2DShadow ShadowMapTexture;

layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec3 position;
in vec4 projectedPos;
in vec2 rippleTexcoord;
out vec4 fragColor;

float computeLightFactor();

void main()
{
	// Calculate the projected tex coords
	vec2 projTexCoord = projectedPos.xy / projectedPos.w * 0.5 + 0.5;

	vec3 normal = vec3(0.0, 0.0, 0.0);

	// Pick up the result of both offsets
	for (int i = 0; i < 2; i++)
	{
		vec2 data = texture(RippleTextures[i], rippleTexcoord + Offsets[i]).xy;
		normal += vec3(data, 1.0 - length(data));
	}

	vec2 perturbation = WaveHeight * normalize(normal).xy;

	vec4 reflection = texture(ReflectionTexture, projTexCoord + perturbation);
	vec4 refraction = texture(RefractionTexture, projTexCoord + perturbation);

	// Calculate the Fresnel factor
	vec3 viewDir = normalize(-position);
	float factor = 0.98 * (1.0 - pow(1.0 - dot(viewDir, ViewNormal), 4.5));

	fragColor = mix(reflection, refraction, factor);
	fragColor = mix(fragColor, vec4(0.25, 0.25, 0.75, 1.0), 0.25);

	// Add the specular highlight
	vec3 lightReflection = reflect(-Light.directionView, normalize(mat3(View) * normal));
	float specular = clamp(pow(dot(normalize(lightReflection), viewDir), 256), 0.0, 1.0);
	fragColor.rgb += specular;
}

