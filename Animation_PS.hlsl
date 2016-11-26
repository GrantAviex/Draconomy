
#include "Deferred/Shaders/ShaderLayouts.hlsli"
struct V_IN
{
	float4 pos		: SV_POSITION;		// position in projection space
	float4 nrm		: NORMAL;
	float4 color	: COL;
	float2 uv		: TEXCOORD;
	float4 worldPos : POSITIONWS;
};

texture2D		baseTexture		: register( t0 );
SamplerState	filters	: register( s0 );


PixelOutputCPN main(V_IN input)
{
	//float4 baseColor = baseTexture.Sample(filters, input.uv); // get base color
	//float4 ambient = { 0.2, 0.2, 0.2, 1 };
	//
	//float3 lightDir = { 1, -1, 1 };
	//lightDir = normalize(lightDir);
	//
	//float3 normal = normalize((float3)input.nrm);
	//
	//	float intensity = clamp(dot(-lightDir, normal), 0, 1);
	//
	//float3 color = (float3)(baseColor);
	//	color.x += input.color.x;
	//color.y += input.color.y;
	//color.z += input.color.z;
	//
	//color *= ambient.xyz;
	//
	//color += saturate(baseColor* intensity).xyz;
	//return float4(color, baseColor.a);


	PixelOutputCPN output;
	output.position = input.worldPos;
	output.color = baseTexture.Sample(filters, input.uv);
	output.color.xyz *= input.color.xyz;
	//output.color.x += input.color.x;
	//output.color.y += input.color.y;
	//output.color.z += input.color.z;

	float4 normal = float4(normalize(input.nrm.xyz), 0);
	output.normal = normal;// SpheremapEncode(normalize(input.nrm.xyz));;
	//output.normal = float3(CartesianToSpherical(normal.xyz), 0);
	//output.depth = input.pos.z;
	return output;
}