//--------------------------------------------------------------------------------------
// File: BasicCompute11.hlsl
//
// This file contains the Compute Shader to perform array A + array B
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define PI 3.14159274f
#define G 9.81f
//cs
RWTexture2D<float> TexOut : register(u0);
cbuffer ConstantBuffer : register(b0)
{
	float t;
	float height;
	float speed;
	float freq;
}
[numthreads(1, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)  // no y,z (N,0,0)
{
	TexOut[DTid.xy] = height *sin(freq*DTid.x+ t * speed/ 100 );
}
//[numthreads(1, 1, 1)]
//void CS(uint index : SV_GroupIndex, uint3 pos : SV_GroupID)  // no y,z (N,0,0)
//{
//	BufferOut[index] = height * sin(freq*pos.x + t * speed / 100);
//}