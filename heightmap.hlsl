StructuredBuffer<float> Height : register(t0);
struct ps_input
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};
ps_input VS(float2 pos : TEXCOORD, uint vid : SV_VertexID)
{
	ps_input output;
	output.pos = float4(pos,0.0f,1.0f);
	//output.pos.y = pos.y+Height[vid];
	output.col.r = 1.0 - Height[vid];
	output.col.g = 1.0 - Height[vid];
	output.col.b = 1.0 - Height[vid];
	output.col.a = 1.0;
	//return mul(float4(pos,1.0f),transform);
	return output;
}

//ps
float4 PS(ps_input input) : SV_Target
{
	//return float4(1.0f, 2.0f, 0.0f, 1.0f);
	return input.col;    // Yellow, with Alpha = 1
}