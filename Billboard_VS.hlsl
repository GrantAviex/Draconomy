struct VS_INPUT
{
	float4	color		: COLOR;
	float4	rect		: RECT;
	float3	pos			: POSITION;
	float2	scale		: SCALE;
	float2	size		: SIZE;
	float2	offset		: OFFSET;
	float	useWorld	: WORLD;
};

VS_INPUT main(VS_INPUT input)
{
	return input;
}