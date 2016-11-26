#include "Deferred/Shaders/ShaderLayouts.hlsli"
texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

struct PS_INPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOOR;
};

float4 main(PS_INPUT input) : SV_TARGET0
{

	//PixelOutputCP output;
	//float4 baseColor = baseTexture.Sample(filters[0], input.uv) * input.color;

	//output.color = baseColor;
	//output.normal = float4(-0,-0,100,100);
	return baseTexture.Sample(filters[0], input.uv) * input.color;
}