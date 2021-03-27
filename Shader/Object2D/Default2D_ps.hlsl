#include "Default2D.hlsli"

SamplerState smp:register(s0);
Texture2D<float4> tex:register(t0);

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(input.uv.x, 1.0f, 1.0f, 1.0f);
	//float4 t = tex.Sample(smp, input.uv);
	//float alpha = color.a * t.a;
	//return float4(color.xyz * t.xyz * alpha, alpha);
	return tex.Sample(smp, input.uv) * color;
}