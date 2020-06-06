#version 330

#include "includes.glsl"

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 ShadowViewProjection;
uniform vec4 ClipPlane;

POSITION in vec4 inPosition;
NORMAL in vec3 inNormal;
MODEL in mat4 inModel;

out vec3 position;
out vec4 positionLight;
out vec3 normal;

void computePosition()
{
	mat4 modelView = View * inModel;

	vec4 viewPos = modelView * inPosition;
	positionLight = ShadowViewProjection * inModel * inPosition;
	gl_Position = Projection * viewPos;

	// This is so we can reuse the shader for water rendering
	gl_ClipDistance[0] = dot(ClipPlane, inModel * inPosition);

	position = viewPos.xyz;
	normal = transpose(inverse(mat3(modelView))) * inNormal;
}
