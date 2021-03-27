#include "Default2D.hlsli"

cbuffer trans:register(b0)
{
	matrix scale;
	matrix trans;
	float2 camera;
}

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;
	pos = mul(rotation, pos);
	pos = float4(pos.xy + coordinate.xy - camera, 0.0f, 1.0f);
	pos = mul(scale, pos);
	pos = mul(trans, pos);
	output.pos = pos;
	output.svpos = pos;
	output.uv = uv;
	return output;
}