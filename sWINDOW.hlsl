Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct ps_input
{
float4 pos:SV_Position;
float2 tex:TEXTURE0;
};

ps_input VS(uint vertexId : SV_VertexID)
{
	ps_input output;
if (vertexId == 0){
	output.pos = float4(-1.0f, 1.0f, 0.0f, 1.0f);
	output.tex = float2(0.0f, 1.0f);
}
else if (vertexId == 1)
{
	output.pos = float4(1.0f, 1.0f, 0.0f, 1.0f);
	output.tex = float2(1.0f, 1.0f);
}
else if (vertexId == 2) {
	output.pos = float4(-1.0f, -1.0f, 0.0f, 1.0f);
	output.tex = float2(0.0f, 0.0f);
}
else if (vertexId == 3) {
	output.pos = float4(1.0f, -1.0f, 0.0f, 1.0f);
	output.tex = float2(1.0f, 0.0f);
}
	return output;
}


float4 PS(ps_input input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.tex);
}
