#version 330

#include "includes.glsl"

void computePosition();

POSITION in vec4 inPosition;
NORMAL in vec3 inNormal;

out vec4 modelPos;

void main()
{
	computePosition();
	modelPos = vec4(inPosition.xyz, inNormal.y);
}
