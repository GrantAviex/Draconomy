#pragma pack_matrix( row_major )

struct GS_INPUT
{
	float4	color		: COLOR;
	float4	rect		: RECT;
	float3	pos			: POSITION;
	float2	scale		: SCALE;
	float2	size		: SIZE;
	float2	offset		: OFFSET;
	float	useWorld	: WORLD;
};

struct RS_INPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOOR;
};

cbuffer ASPECT_RATIO : register(b0)
{
	float4 ar;
};

cbuffer SCENE  : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer WORLD : register (b2)
{
	float4x4 worldMatrix;
}

[maxvertexcount(4)]
void main(point GS_INPUT input[1], inout TriangleStream< RS_INPUT > output)
{
	float xOff = input[0].size.x * input[0].scale.x;
	float yOff = (input[0].size.y * input[0].scale.y) * ar.x;
	
	// v0 = top left
	// v1 = top right
	// v2 = bot left
	// v3 = bot right
	RS_INPUT verts[4];

	[unroll(4)] for (uint i = 0; i < 4; i++)
	{
		verts[i].pos = float4(input[0].pos, 1);
		verts[i].color = input[0].color;
	}

	
	[branch]if (input[0].useWorld == 0.f)
	{
		[unroll(4)] for (uint e = 0; e < 4; e++)
		{
			verts[e].pos = mul(verts[e].pos, viewMatrix);
			verts[e].pos = mul(verts[e].pos, projectionMatrix);
		}

		verts[0].pos.x -= xOff;
		verts[0].pos.y += yOff;
		verts[1].pos.x += xOff;
		verts[1].pos.y += yOff;
		verts[2].pos.x -= xOff;
		verts[2].pos.y -= yOff;
		verts[3].pos.x += xOff;
		verts[3].pos.y -= yOff;


		// 2D offset
		verts[0].pos.x += input[0].offset.x;
		verts[1].pos.x += input[0].offset.x;
		verts[2].pos.x += input[0].offset.x;
		verts[3].pos.x += input[0].offset.x;

		verts[0].pos.y += input[0].offset.y;
		verts[1].pos.y += input[0].offset.y;
		verts[2].pos.y += input[0].offset.y;
		verts[3].pos.y += input[0].offset.y;
	}
	else
	{
		verts[0].pos.x -= xOff;
		verts[0].pos.y += yOff;
		verts[1].pos.x += xOff;
		verts[1].pos.y += yOff;
		verts[2].pos.x -= xOff;
		verts[2].pos.y -= yOff;
		verts[3].pos.x += xOff;
		verts[3].pos.y -= yOff;


		// 2D offset
		verts[0].pos.x += input[0].offset.x;
		verts[1].pos.x += input[0].offset.x;
		verts[2].pos.x += input[0].offset.x;
		verts[3].pos.x += input[0].offset.x;

		verts[0].pos.y += input[0].offset.y;
		verts[1].pos.y += input[0].offset.y;
		verts[2].pos.y += input[0].offset.y;
		verts[3].pos.y += input[0].offset.y;


		[unroll(4)] for (uint e = 0; e < 4; e++)
		{
			verts[e].pos = mul(verts[e].pos, worldMatrix);
			verts[e].pos = mul(verts[e].pos, viewMatrix);
			verts[e].pos = mul(verts[e].pos, projectionMatrix);
		}
	}
	

	// what part of image to draw
	// top
	verts[0].uv = float2(input[0].rect.x,input[0].rect.y);			// 0,0 top left
	verts[1].uv = float2(input[0].rect.z,input[0].rect.y);			// 1,0 top right

	//bottom
	verts[2].uv = float2(input[0].rect.x, input[0].rect.w);			// 0,1 bottom left
	verts[3].uv = float2(input[0].rect.z,input[0].rect.w);			// 1,1 bottom right

	//verts[0].uv = float2(input[0].rect.x, input[0].rect.y);
	//verts[1].uv = float2(input[0].rect.w,input[0].rect.x);
	//
	////bottom
	//verts[2].uv = float2(input[0].rect.y, input[0].rect.z);
	//verts[3].uv = float2(input[0].rect.w, input[0].rect.z);

	output.Append(verts[3]);
	output.Append(verts[1]);
	output.Append(verts[2]);
	output.Append(verts[0]);
}