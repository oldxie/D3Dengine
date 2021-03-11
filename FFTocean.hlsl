#define PI 3.14159274f
#define G 9.81f
#define N 128.0f
#define Lambda 1
#define OceanLength 128.0f
//cs
RWTexture2D<float> TexOut : register(u0);
RWTexture2D<float4> GaussianRandomRT : register(u1);
RWTexture2D<float4> HeightSpectrumRT: register(u2);
RWTexture2D<float4> DisplaceXSpectrumRT: register(u3);
RWTexture2D<float4> DisplaceZSpectrumRT: register(u4);
RWTexture2D<float4> DisplaceRT: register(u5);
RWTexture2D<float4> NormalRT: register(u6);
RWTexture2D<float4> BubblesRT: register(u7);

Texture2D<float4> GaussianRandomInput : register(t0);
Texture2D<float4> HeightSpectrumInput : register(t1);
Texture2D<float4> DisplaceXSpectrumInput: register(t2);
Texture2D<float4> DisplaceZSpectrumInput: register(t3);
Texture2D<float4> DisplaceInput: register(t4);

//FFT 
Texture2D<float4> InputRT : register(t6);
RWTexture2D<float4> OutputRT: register(u6);


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
	uint Ns;
	
	//float OceanLength;		//海洋长度
	float BubblesScale;	    //泡沫强度
	float BubblesThreshold; //泡沫阈值

}



[numthreads(8, 8, 1)]
void ComputeGaussianRandom(uint3 id: SV_DispatchThreadID)
{
	float2 g = gaussian(id.xy);

	GaussianRandomRT[id.xy] = float4(g,0,0);
}

//生成高度频谱
[numthreads(8, 8, 1)]
void CreateHeightSpectrum(uint3 id: SV_DispatchThreadID)
{
	float2 k = float2(2.0f * PI * id.x / N - PI, 2.0f * PI * id.y / N - PI);
	//k = float2(id.x / N, id.y / N);
	float2 gaussian = GaussianRandomInput[id.xy].xy;
	
	float2 hTilde0 = gaussian * sqrt(abs(phillips(k) * DonelanBannerDirectionalSpreading(k)) / 2.0f);
	float2 hTilde0Conj = gaussian * sqrt(abs(phillips(-k) * DonelanBannerDirectionalSpreading(-k)) / 2.0f);
	hTilde0Conj.y *= -1.0f;

	float omegat = dispersion(k) * Time;
	float c = cos(omegat);
	float s = sin(omegat);

	float2 h1 = complexMultiply(hTilde0, float2(c, s));
	float2 h2 = complexMultiply(hTilde0Conj, float2(c, -s));

	float2 HTilde = h1 + h2;
	//HTilde = float2(0.3, 0.3);

	HeightSpectrumRT[id.xy] = float4(HTilde,  0, 0);
}
//生成偏移频谱
[numthreads(8, 8, 1)]
void CreateDisplaceSpectrum(uint3 id: SV_DispatchThreadID)
{
	float2 k = float2(2 * PI * id.x / N - PI, 2 * PI * id.y / N - PI);
	k /= max(0.001f, length(k));
	float2 HTilde = HeightSpectrumInput[id.xy].xy;

	float2 KxHTilde = complexMultiply(float2(0, -k.x), HTilde);
	float2 kzHTilde = complexMultiply(float2(0, -k.y), HTilde);

	DisplaceXSpectrumRT[id.xy] = float4(KxHTilde, 0, 0);
	DisplaceZSpectrumRT[id.xy] = float4(kzHTilde, 0, 0);
}

//FFT
//横向FFT计算,只针对第m-1阶段，最后一阶段需要特殊处理
[numthreads(8, 8, 1)]
void FFTHorizontal(uint3 id: SV_DispatchThreadID)
{
	int2 idxs = id.xy;
	idxs.x = floor(id.x / (Ns * 2.0f)) * Ns + id.x % Ns;
	float angle = 2.0f * PI * (id.x / (Ns * 2.0f));
	float2 w = float2(cos(angle), sin(angle));

	float2 x0 = InputRT[idxs].xy;
	float2 x1 = InputRT[int2(idxs.x + N * 0.5f, idxs.y)].xy;

	float2 output = x0 + float2(w.x * x1.x - w.y * x1.y, w.x * x1.y + w.y * x1.x);
	OutputRT[id.xy] = float4(output, 0, 0);
}
//横向FFT最后阶段计算,需要进行特别处理
[numthreads(8, 8, 1)]
void FFTHorizontalEnd(uint3 id: SV_DispatchThreadID)
{
	int2 idxs = id.xy;
	idxs.x = floor(id.x / (Ns * 2.0f)) * Ns + id.x % Ns;
	float angle = 2.0f * PI * (id.x / (Ns * 2.0f));
	float2 w = float2(cos(angle), sin(angle));

	/*********修改内容***********/
	w *= -1;
	/***************************/

	float2 x0 = InputRT[idxs].xy;
	float2 x1 = InputRT[int2(idxs.x + N * 0.5f, idxs.y)].xy;

	float2 output = x0 + float2(w.x * x1.x - w.y * x1.y, w.x * x1.y + w.y * x1.x);
	/*********修改内容***********/
	int x = id.x - N * 0.5f;
	output *= ((x + 1) % 2.0f) * 1 + (x % 2.0f) * (-1);
	/***************************/
	OutputRT[id.xy] = float4(output, 0, 0);
}
//纵向FFT计算,只针对第m-1阶段，最后一阶段需要特殊处理
[numthreads(8, 8, 1)]
void FFTVertical(uint3 id: SV_DispatchThreadID)
{
	int2 idxs = id.xy;
	idxs.y = floor(id.y / (Ns * 2.0f)) * Ns + id.y % Ns;
	float angle = 2.0f * PI * (id.y / (Ns * 2.0f));
	float2 w = float2(cos(angle), sin(angle));

	float2 x0 = InputRT[idxs].xy;
	float2 x1 = InputRT[int2(idxs.x, idxs.y + N * 0.5f)].xy;

	float2 output = x0 + float2(w.x * x1.x - w.y * x1.y, w.x * x1.y + w.y * x1.x);
	OutputRT[id.xy] = float4(output, 0, 0);
}
//纵向FFT最后阶段计算,需要进行特别处理
[numthreads(8, 8, 1)]
void FFTVerticalEnd(uint3 id: SV_DispatchThreadID)
{
	int2 idxs = id.xy;
	idxs.y = floor(id.y / (Ns * 2.0f)) * Ns + id.y % Ns;
	float angle = 2.0f * PI * (id.y / (Ns * 2.0f));
	float2 w = float2(cos(angle), sin(angle));

	/*********修改内容***********/
	w *= -1;
	/***************************/

	float2 x0 = InputRT[idxs].xy;
	float2 x1 = InputRT[int2(idxs.x, idxs.y + N * 0.5f)].xy;

	float2 output = x0 + float2(w.x * x1.x - w.y * x1.y, w.x * x1.y + w.y * x1.x);
	/*********修改内容***********/
	int x = id.y - N * 0.5f;
	output *= ((x + 1) % 2.0f) * 1 + (x % 2.0f) * (-1);
	/***************************/
	OutputRT[id.xy] = float4(output, 0, 0);
}
//生成偏移纹理
[numthreads(8, 8, 1)]
void TextureGenerationDisplace(uint3 id: SV_DispatchThreadID)
{
	float y = length(HeightSpectrumInput[id.xy].xy) / (N * N) * height *300 ;//高度
	float x = length(DisplaceXSpectrumInput[id.xy].xy) / (N * N) * Lambda*50 ;//x轴偏移
	float z = length(DisplaceZSpectrumInput[id.xy].xy) / (N * N) * Lambda*50 ;//z轴偏移

	HeightSpectrumRT[id.xy] = float4(y, y, y, 0);
	DisplaceXSpectrumRT[id.xy] = float4(x, x, x, 0);
	DisplaceZSpectrumRT[id.xy] = float4(z, z, z, 0);
	DisplaceRT[id.xy] = float4(x, y, z, 0);
}

//生成法线和泡沫纹理
[numthreads(8, 8, 1)]
void TextureGenerationNormalBubbles(uint3 id: SV_DispatchThreadID)
{
	//计算法线
	float uintLength = OceanLength / (N - 1.0f);//两点间单位长度
	//获取当前点，周围4个点的uv坐标
	uint2 uvX1 = uint2((id.x - 1.0f + N) % N, id.y);
	uint2 uvX2 = uint2((id.x + 1.0f + N) % N, id.y);
	uint2 uvZ1 = uint2(id.x, (id.y - 1.0f + N) % N);
	uint2 uvZ2 = uint2(id.x, (id.y + 1.0f + N) % N);

	
	//以当前点为中心，获取周围4个点的偏移值
	float3 x1D = DisplaceInput[uvX1].xyz;//在x轴 第一个点的偏移值
	float3 x2D = DisplaceInput[uvX2].xyz;//在x轴 第二个点的偏移值
	float3 z1D = DisplaceInput[uvZ1].xyz;//在z轴 第一个点的偏移值
	float3 z2D = DisplaceInput[uvZ2].xyz;//在z轴 第二个点的偏移值
	
	//以当前点为原点，构建周围4个点的坐标
	float3 x1 = float3(x1D.x - uintLength, x1D.yz);//在x轴 第一个点的坐标
	float3 x2 = float3(x2D.x + uintLength, x2D.yz);//在x轴 第二个点的坐标
	float3 z1 = float3(z1D.xy, z1D.z - uintLength);//在z轴 第一个点的坐标
	float3 z2 = float3(z1D.xy, z1D.z + uintLength);//在z轴 第二个点的坐标

	//计算两个切向量
	float3 tangentX = x2 - x1;
	float3 tangentZ = z2 - z1;

	
	//计算法线
	float3 normal = normalize(cross(tangentZ, tangentX));
	//计算泡沫
	float3 ddx = x2D - x1D;
	float3 ddz = z2D - z1D;
	//雅可比行列式
	float jacobian = (1.0f + ddx.x) * (1.0f + ddz.z) - ddx.z * ddz.x;

	jacobian = saturate(max(0, BubblesThreshold - saturate(jacobian)) * BubblesScale);

	NormalRT[id.xy] = float4(normal, 0);
	BubblesRT[id.xy] = float4(jacobian, jacobian, jacobian, 0);
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
	rngState = wangHash(id.y * N + id.x);
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
float phillips(float2 k)
{
	float kLength = length(k);
	
	kLength = max(0.001f, kLength);
	
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

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)  // no y,z (N,0,0)
{
	TexOut[DTid.xy] = TexOut[DTid.xy] + height * sin(freq*DTid.y + Time * speed / 100);
}