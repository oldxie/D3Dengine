cbuffer CBuf 
{
	matrix transform;
};

float4 VS(float3 pos : Position , uint vid : SV_VertexID) : SV_Position
{
	return mul( float4(pos,1.0f),transform );
}