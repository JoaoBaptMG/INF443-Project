#version 330

uniform float TextureScale;
uniform float DistanceFalloff;
uniform sampler2D CloudTexture;
uniform vec2 Displacement;

in vec4 position;
out vec4 fragColor;

void main()
{
	// Compute the texcoord (and its gradients)
	vec2 texcoord = position.xz / position.w / TextureScale;
	vec4 dpdx = dFdx(position), dpdy = dFdy(position);
	vec2 dtdx = (dpdx.xz * position.w - position.xz * dpdx.w) / (position.w * position.w) / TextureScale;
	vec2 dtdy = (dpdy.xz * position.w - position.xz * dpdy.w) / (position.w * position.w) / TextureScale;

	// Sample the texture in 6 octaves
	float power = 0.5;
	float v = 0.0;

	for (int i = 0; i < 6; i++)
	{
		v += power * textureGrad(CloudTexture, texcoord + Displacement, dtdx, dtdy).r;

		power /= 2.0;
		texcoord *= 2.0;
		dtdx *= 2.0;
		dtdy *= 2.0;
	}

	float alpha = clamp(v, 0.0, 1.0) * exp(-length(texcoord) / DistanceFalloff);
	fragColor = vec4(1.0, 1.0, 1.0, pow(alpha, 0.875) * 0.75);
}