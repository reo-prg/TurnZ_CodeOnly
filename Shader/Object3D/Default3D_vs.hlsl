#include "Default3D.hlsli"

cbuffer wvp:register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

VSOutput main(float4 pos : POSITION, float4 normal : NORMAL)
{
	VSOutput output;
	pos = float4(pos.xyz * size.xyz, 1.0f);
	pos = mul(rotation, pos);
	pos = float4(pos.xyz + coordinate.xyz, 1.0f);
	pos = mul(world, pos);
	output.pos = pos;
	output.svpos = mul(mul(projection, view), pos);
	output.normal = normal;
	return output;
}