#version 330

#include "includes.glsl"

void computePosition();

COLOR in vec4 inColor;

out vec4 color;

void main()
{
	computePosition();
	color = inColor;
}
