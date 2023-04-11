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

VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Color = Color;
	return (output);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.Color;
}
cbuffer ConstantBuffer : register (b0)
{
	matrix	World;
	matrix	View;
	matrix  Projection;
}