#pragma pack_matrix( row_major )

struct V_IN
{
	float3 pos		: POSITION;			// position in local space
	float3 nrm		: NORMAL;
	float3 uvw		: TEXCOORD;

	float4 WMr0		: WM0;
	float4 WMr1		: WM1;
	float4 WMr2		: WM2;
	float4 WMr3		: WM3;

	float4 color	: COLOR;
	float4 padding0	: PADDING0;
	float4 padding1	: PADDING1;
	float4 padding2	: PADDING2;
};

struct V_OUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};

cbuffer SCENE  : register( b0 )
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

V_OUT main( V_IN input )
{
	V_OUT output = (V_OUT) 0;

	// ensures translation is preserved during matrix multiply  
	float4 localH = float4( input.pos, 1 );
	
	// Construct the world space matrix 
	float4x4 worldMatrix = float4x4( 
	float4( input.WMr0.x, input.WMr0.y, input.WMr0.z, input.WMr0.w ),
	float4( input.WMr1.x, input.WMr1.y, input.WMr1.z, input.WMr1.w ),
	float4( input.WMr2.x, input.WMr2.y, input.WMr2.z, input.WMr2.w ),
	float4( input.WMr3.x, input.WMr3.y, input.WMr3.z, input.WMr3.w )	);

	//	input.WMr1, input.WMr2, input.WMr3 );

	// move local space vertex from vertex buffer into world space.
	localH			= mul( localH, worldMatrix );

	// Move into view space, then projection space
	localH		= mul( localH, viewMatrix );
	localH		= mul( localH, projectionMatrix );
	output.pos	= localH;

	output.color = input.color;

	return output; // send projected vertex to the rasterizer stage
}








