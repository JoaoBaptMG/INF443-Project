#version 330

#include "includes.glsl"

uniform mat4 View;
uniform mat4 Projection;

POSITION in vec4 inPosition;
out vec4 position;

void main()
{
	position = inPosition;
	gl_Position = Projection * View * inPosition;
}