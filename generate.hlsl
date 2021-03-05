#pragma once

//cbuffer CBuf
//{
//	matrix transform;
//};

//vs
struct VertexOut
{
	float3 PosL    : POSITION;
};
VertexOut VS(float3 pos : Position)
{
	VertexOut vout;
	vout.PosL = pos;
	return vout;
}

//hs
cbuffer CBuf
{
	uint meshsize;
};
struct PatchTess
{
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
	// Additional info you want associated per patch.
};
PatchTess ConstantHS(InputPatch<VertexOut, 4> patch,
	uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	// Uniformly tessellate the patch 3 times.
	pt.EdgeTess[0] = meshsize; // Left edge
	pt.EdgeTess[1] = meshsize; // Top edge
	pt.EdgeTess[2] = meshsize; // Right edge
	pt.EdgeTess[3] = meshsize; // Bottom edge
	pt.InsideTess[0] = meshsize; // u-axis (columns)
	pt.InsideTess[1] = meshsize; // v-axis (rows)
	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosL = p[i].PosL;

	return hout;
}

struct DomainOut
{
	float4 PosL : SV_POSITION;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("quad")]
DomainOut DS(PatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<HullOut, 4> quad)
{
	DomainOut dout;
	// Bilinear interpolation.
	float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
	float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
	float3 pos = lerp(v1, v2, uv.y);
	dout.PosL = float4(pos, 1.0f);
	//dout.PosH=mul(float4(pos, 1.0f), transform);
	//dout.PosL = pos;
	return dout;
}

////gs
//struct GeometryOut
//{
//	float3 PosL : POSITION;
//};
//
//[maxvertexcount(9)]
//void GS(triangle DomainOut input[3], inout TriangleStream<GeometryOut> TriStream)
//{
//	int i;
//	for (i = 0; i < 3; ++i)
//	{
//		TriStream.Append(input[i]);
//	}
//	TriStream.RestartStrip();
//}