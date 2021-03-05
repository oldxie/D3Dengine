//vs
cbuffer CBuf
{
	matrix transform;
};
Texture2D  Height : register(t0);
//StructuredBuffer<float> Height : register(t0);
SamplerState g_SamLinear : register(s0);
struct VertexOut
{
	float4 PosL    : SV_POSITION;
};
VertexOut VS(float3 pos : Position)
{
	VertexOut vout;
	float2 tex;
	tex.x = (pos.x + 64.0f) / 128.0f;
	tex.y = (pos.z + 64.0f) / 128.0f;
	
	//pos.y = Height[tex] + Height.Sample(g_SamLinear, tex);
	pos.y = Height.GatherRed(g_SamLinear, tex);
	//pos.x = pos.x + pos.y;
	//pos.z = pos.z + pos.y;
	vout.PosL = mul(float4(pos,1.0f), transform);
	//vout.PosL = float4(pos.y, tex, 1.0f);
	return vout;
}

//ps
float4 PS(VertexOut pin) : SV_Target
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);    // Yellow, with Alpha = 1
}