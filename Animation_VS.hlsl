#pragma pack_matrix( row_major )
#define NUM_BONES 100
struct V_IN
{
	float3 pos	: POSITION;			// position in local space
	float3 nrm	: NORMAL;
	float2 uvw	: TEXCOORD;
	float4  wgt : WEIGHT;
	int4 jind : JOINTINDEX;
};

struct V_OUT
{
	float4 pos		: SV_POSITION;		// position in projection space
	float4 nrm		: NORMAL;
	float4 color		: COL;
	float2 uv		: TEXCOORD;
	float4 worldPos : POSITIONWS;
};

cbuffer SCENE  : register( b0 )
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};
cbuffer WORLDM : register( b1 )
{
	float4x4 worldMatrix;
};
cbuffer JOINT : register(b2)
{
	float4x4 bones[NUM_BONES];
}
cbuffer COLOR : register(b3)
{
	float4 col;
}

V_OUT main( V_IN input )
{
	V_OUT output = (V_OUT) 0;

	// ensures translation is preserved during matrix multiply  

	output.pos =  mul(float4(input.pos, 1), bones[input.jind.x]) * input.wgt.x;
	output.pos += mul(float4(input.pos, 1), bones[input.jind.y]) * input.wgt.y;
	output.pos += mul(float4(input.pos, 1), bones[input.jind.z]) * input.wgt.z;
	output.pos += mul(float4(input.pos, 1), bones[input.jind.w]) * input.wgt.w;


	// move local space vertex from vertex buffer into world space.
	output.pos = mul(output.pos, worldMatrix);
	output.worldPos = output.pos;
	// Move into view space, then projection space
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);
	 

	output.uv	= input.uvw.xy;
	output.nrm = mul(float4(input.nrm, 0), worldMatrix);
	output.color = col;

	return output; // send projected vertex to the rasterizer stage
}