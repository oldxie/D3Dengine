#define PI 3.14159274f
#define G 9.81f
#define N 40
//cs
RWTexture2D<float> TexOut : register(u0);
RWTexture2D<float2> GaussianRandomRT : register(u0);
Texture2D<float2> GaussianRandomInput : register(t0);
RWTexture2D<float4> HeightSpectrumRT: register(u0);;

//declartion
float2 gaussian(float2 id);
float DonelanBannerDirectionalSpreading(float2 k);
float phillips(float2 k);
float dispersion(float2 k);
float2 complexMultiply(float2 c1, float2 c2);

cbuffer ConstantBuffer : register(b0)
{
	float Time;
	float height;
	float speed;
	float freq;

	float4 WindAndSeed;

}



[numthreads(1, 1, 1)]
void ComputeGaussianRandom(uint3 id: SV_DispatchThreadID)
{
	float2 g = gaussian(id.xy);

	GaussianRandomRT[id.xy] = g;
}

//生成高度频谱
[numthreads(1, 1, 1)]
void CreateHeightSpectrum(uint3 id: SV_DispatchThreadID)
{
	float2 k = float2(2.0f * PI * id.x / N - PI, 2.0f * PI * id.y / N - PI);
	//k = float2(id.x / N, id.y / N);
	float2 gaussian = GaussianRandomInput[id.xy].xy;
	
	float2 hTilde0 = gaussian * sqrt(abs(phillips(k) * DonelanBannerDirectionalSpreading(k)) / 2.0f);
	float2 hTilde0Conj = gaussian * sqrt(abs(phillips(-k) * DonelanBannerDirectionalSpreading(-k)) / 2.0f);
	hTilde0Conj.y *= -1.0f;

	float omegat = dispersion(k) * Time/60;
	float c = cos(omegat);
	float s = sin(omegat);

	float2 h1 = complexMultiply(hTilde0, float2(c, s));
	float2 h2 = complexMultiply(hTilde0Conj, float2(c, -s));

	float2 HTilde = h1 + h2;
	//HTilde = float2(0.3, 0.3);

	HeightSpectrumRT[id.xy] = float4(HTilde,  0, 0);
}


//随机种子
uint wangHash(uint seed)
{
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	return seed;
}
//计算均匀分布随机数[0,1)
float rand(uint rngState)
{
	// Xorshift算法
	rngState ^= (rngState << 13);
	rngState ^= (rngState >> 17);
	rngState ^= (rngState << 5);
	return rngState / 4294967296.0f;;
}

//计算高斯随机数
float2 gaussian(float2 id)
{
	//均匀分布随机数
	uint rngState;
	rngState = wangHash(id.y * 128 + id.x);
	float x1 = rand(rngState);
	float x2 = rand(rngState);

	x1 = max(1e-6f, x1);
	x2 = max(1e-6f, x2);
	//计算两个相互独立的高斯随机数
	float g1 = sqrt(-2.0f * log(x1)) * cos(2.0f * PI * x2);
	float g2 = sqrt(-2.0f * log(x1)) * sin(2.0f * PI * x2);

	return float2(g1, g2);
}
//计算弥散
float dispersion(float2 k)
{
	return sqrt(G * length(k));
}
//复数相乘
float2 complexMultiply(float2 c1, float2 c2)
{
	return float2(c1.x * c2.x - c1.y * c2.y,
		c1.x * c2.y + c1.y * c2.x);
}

//Donelan-Banner方向拓展
float DonelanBannerDirectionalSpreading(float2 k)
{
	float betaS;
	float omegap = 0.855f * G / length(WindAndSeed.xy);
	float ratio = dispersion(k) / omegap;

	if (ratio < 0.95f)
	{
		betaS = 2.61f * pow(ratio, 1.3f);
	}
	if (ratio >= 0.95f && ratio < 1.6f)
	{
		betaS = 2.28f * pow(ratio, -1.3f);
	}
	if (ratio > 1.6f)
	{
		float epsilon = -0.4f + 0.8393f * exp(-0.567f * log(ratio * ratio));
		betaS = pow(10, epsilon);
	}
	float theta = atan2(k.y, k.x) - atan2(WindAndSeed.y, WindAndSeed.x);

	return betaS / max(1e-7f, 2.0f * tanh(betaS * PI) * pow(cosh(betaS * theta), 2));
}
//计算phillips谱
float phillips(float2 k1)
{
	vector k = float4(2,2,0,0);
	//return normalize(k);
	float kLength = length(normalize(k));
	return kLength;
	kLength = max(0.001f, kLength);
	// kLength = 1;
	
	float kLength2 = kLength * kLength;
	float kLength4 = kLength2 * kLength2;

	float windLength = length(WindAndSeed.xy);
	float  l = windLength * windLength / G;
	float l2 = l * l;

	float damping = 0.001f;
	float L2 = l2 * damping * damping;
	
	//return exp(-1.0f / (kLength2 * l2));
	//phillips谱
	return  height * exp(-1.0f / (kLength2 * l2)) / kLength4 * exp(-kLength2 * L2);
}

[numthreads(1, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)  // no y,z (N,0,0)
{
	TexOut[DTid.xy] = TexOut[DTid.xy] + height * sin(freq*DTid.y + Time * speed / 100);
}