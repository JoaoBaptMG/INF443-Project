#version 330

#include "includes.glsl"

uniform mat4 Projection;
uniform mat4 View;
uniform float RepeatPeriod;
uniform mat4 ShadowViewProjection;

POSITION in vec4 inPosition;

out vec3 position;
out vec4 positionLight;
out vec4 projectedPos;
out vec2 rippleTexcoord;

void main()
{
	mat4 modelView = View; // Model is the identity matrix

	vec4 viewPos = modelView * inPosition;
	position = viewPos.xyz;
	positionLight = ShadowViewProjection * inPosition;
	projectedPos = Projection * viewPos;
	rippleTexcoord = inPosition.xz / RepeatPeriod;

	gl_Position = projectedPos;
}
