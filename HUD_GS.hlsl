
struct V_IN
{
	float3	pos			: POSITION;
	uint	vertInd		: VERT_ID;
};

struct V_OUT
{
	float4	pos			: SV_POSITION;
	float2	uv			: TEXCOORD;
	uint	texArrIndex : TEX_ARR_IND;
};

struct Quad
{
	float leftPos;
	float topPos;
	float rightPos;
	float botPos;

	float leftTexCoord;
	float topTexCoord;
	float rightTexCoord;
	float botTexCoord;

	float depth;
	uint  texArrIndex;
};

StructuredBuffer<Quad> quadInfo : register( t0 );

// disregard the vertex information, we only need the id of the current vertex being processed
// Access the structured buffer with the vertex id and use that info to construct the quad
[ maxvertexcount( 4 ) ]
void main( point V_IN input[ 1 ], inout TriangleStream< V_OUT > output )
{
	// [ 0 ] topLeft	[ 1 ] topRight	[ 2 ] botLeft	[ 3 ] botRight
	V_OUT verts[ 4 ];

	[unroll(4)] for( uint i = 0; i < 4; i++ )
	{
		verts[ i ].pos		= float4( 0.0f, 0.0f, 0.0f, 0.0f );
		verts[ i ].uv		= float2( 0.0f, 0.0f );
	}
	
	// Format position
	verts[ 0 ].pos =  float4( 
		quadInfo[ input[ 0 ].vertInd ].leftPos, 
		quadInfo[ input[ 0 ].vertInd ].topPos, 
		quadInfo[ input[ 0 ].vertInd ].depth, 
		1.0f );

	verts[ 1 ].pos =  float4( 
		quadInfo[ input[ 0 ].vertInd ].rightPos,
		quadInfo[ input[ 0 ].vertInd ].topPos,
		quadInfo[ input[ 0 ].vertInd ].depth,
		1.0f );

	verts[ 2 ].pos =  float4( 
		quadInfo[ input[ 0 ].vertInd ].leftPos, 
		quadInfo[ input[ 0 ].vertInd ].botPos, 
		quadInfo[ input[ 0 ].vertInd ].depth, 
		1.0f );

	verts[ 3 ].pos =  float4( 
		quadInfo[ input[ 0 ].vertInd ].rightPos, 
		quadInfo[ input[ 0 ].vertInd ].botPos, 
		quadInfo[ input[ 0 ].vertInd ].depth, 
		1.0f );

	// Format uvs
	verts[ 0 ].uv =  float2( 
		quadInfo[ input[ 0 ].vertInd ].leftTexCoord,
		quadInfo[ input[ 0 ].vertInd ].topTexCoord );

	verts[ 1 ].uv =  float2( 
		quadInfo[ input[ 0 ].vertInd ].rightTexCoord,
		quadInfo[ input[ 0 ].vertInd ].topTexCoord );

	verts[ 2 ].uv =  float2( 
		quadInfo[ input[ 0 ].vertInd ].leftTexCoord,
		quadInfo[ input[ 0 ].vertInd ].botTexCoord );

	verts[ 3 ].uv =  float2( 
		quadInfo[ input[ 0 ].vertInd ].rightTexCoord,
		quadInfo[ input[ 0 ].vertInd ].botTexCoord );

	verts[ 0 ].texArrIndex = quadInfo[ input[ 0 ].vertInd ].texArrIndex;
	verts[ 1 ].texArrIndex = quadInfo[ input[ 0 ].vertInd ].texArrIndex;
	verts[ 2 ].texArrIndex = quadInfo[ input[ 0 ].vertInd ].texArrIndex;
	verts[ 3 ].texArrIndex = quadInfo[ input[ 0 ].vertInd ].texArrIndex;

	output.Append( verts[ 0 ] );
	output.Append( verts[ 1 ] );
	output.Append( verts[ 2 ] );
	output.Append( verts[ 3 ] );
}