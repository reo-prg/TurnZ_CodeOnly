cbuffer matrixx:register(b1)
{
	float4 coordinate;
	float4 color;
	float4 size;
	matrix rotation;
}

struct VSOutput
{
	float4 svpos:SV_POSITION;
	float4 pos:POSITION;
	float4 normal:NORMAL;
};