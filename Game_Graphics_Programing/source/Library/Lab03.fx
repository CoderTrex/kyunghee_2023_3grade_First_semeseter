float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	return float4 (1.0f, 1.0f, 0.0f, 1.0f);
}

float4 VS(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}

struct VS_OUTPUT
{
	float4  Pos : SV_POSITION;
	float4	Color : COLOR0;
};

cbuffer ConstantBuffer : register (b0)
{
	matrix	World;
	matrix	View;
	matrix  Projection;
}