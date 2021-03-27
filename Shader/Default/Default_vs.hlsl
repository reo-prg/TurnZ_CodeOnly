#include "Default.hlsli"

cbuffer wvp:register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

VSOutput main(float4 pos : POSITION)
{
	VSOutput output;
	pos = float4(pos.xyz * size.xyz, 1.0f);
	pos = mul(rotation, pos);
	pos = float4(pos.xyz + coordinate.xyz, 1.0f);
	pos = mul(world, pos);
	output.pos = pos;
	output.svpos = mul(mul(projection, view), pos);
	return output;
}