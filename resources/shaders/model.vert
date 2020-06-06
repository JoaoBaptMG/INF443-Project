#version 330

#include "includes.glsl"

const int TotalNumBones = 128;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 ShadowViewProjection;
uniform vec4 ClipPlane;
uniform mat4 Bones[TotalNumBones];

POSITION in vec4 inPosition;
NORMAL in vec3 inNormal;
MODEL in mat4 Model;
TEXCOORD in vec2 inTexcoord[1];
in ivec4 inBoneIDs;
in vec4 inBoneWeights;

out vec3 position;
out vec4 positionLight;
out vec3 normal;
out vec2 texcoord;

void computePosition()
{
	mat4 bone = Bones[inBoneIDs.x] * inBoneWeights.x;
	bone += Bones[inBoneIDs.y] * inBoneWeights.y;
	bone += Bones[inBoneIDs.z] * inBoneWeights.z;
	bone += Bones[inBoneIDs.w] * inBoneWeights.w;

	mat4 boneModel = Model * bone;
	mat4 modelView = View * boneModel;

	vec4 viewPos = modelView * inPosition;
	positionLight = ShadowViewProjection * boneModel * inPosition;
	gl_Position = Projection * viewPos;

	// This is so we can reuse the shader for water rendering
	gl_ClipDistance[0] = dot(ClipPlane, boneModel * inPosition);

	position = viewPos.xyz;
	normal = transpose(inverse(mat3(modelView))) * inNormal;
}

void main()
{
	computePosition();
	texcoord = inTexcoord[0];
}
