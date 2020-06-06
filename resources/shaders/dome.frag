#version 330

in vec3 position;

uniform mat4 View;
uniform vec4 HorizonColor;
uniform vec4 PinnacleColor;
uniform float SphereRadius;

out vec4 fragColor;

void main()
{
	vec4 modelPos = inverse(View) * vec4(position, 1.0);
	fragColor = mix(HorizonColor, PinnacleColor, modelPos.y / SphereRadius);
}