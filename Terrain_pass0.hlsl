#define _FresnelScale 0.5
#define _OceanColorShallow float4(1,1,1,1)
#define _OceanColorDeep float4(1,1,1,1)
#define _LightColor0 float4(1,1,1,1)
#define _BubblesColor float4(1,1,1,1)
#define _Specular float4(1,1,1,1)
#define _Gloss 20.f
//vs
cbuffer CBuf
{
	matrix transform;
	matrix m;
	matrix v;
	matrix p;
	float3 camapos;

};
Texture2D<float4>  Height : register(t0);
SamplerState g_SamLinear : register(s0);

Texture2D<float4>  _Normal : register(t0);
Texture2D<float4>  _Bubbles : register(t1);
TextureCube  _Sky : register(t2);
SamplerState ps_SamLinear : register(s0);
RWTexture2D<float4>  Debug : register(u1);
struct VertexOut
{
	float4 PosL    : SV_POSITION;
	float2 uv: TEXCOORD0;
	float3 worldPos: TEXCOORD1;
};
VertexOut VS(float3 pos : Position)
{
	VertexOut vout;
	float2 tex;
	/*tex.x = (pos.x + 256.0f) / 512.0f;
	tex.y = (pos.z + 256.0f) / 512.0f;*/
	tex.x = (pos.x + 64.0f) / 128.0f;
	tex.y = (pos.z + 64.0f) / 128.0f;
	pos = pos + Height.GatherRed(g_SamLinear, tex).xyz ;
	vout.PosL = mul(float4(pos,1.0f), transform);
	vout.uv = tex;
	vout.worldPos = mul(m,float4(pos,1.0f)).xyz;
	return vout;
}


float4 PS(VertexOut i) :Sv_Target
{
	float3 normal = _Normal.Sample(ps_SamLinear, i.uv).rgb;
	float bubbles = _Bubbles.Sample(ps_SamLinear, i.uv).r *12 +0.25;
	float3 lightDir = normalize(float3(-120,-140,100)-i.worldPos);
	float3 viewDir = normalize(float3(camapos)-i.worldPos);
	float3 reflectDir = reflect(-viewDir, normal);
	//float4 testcol = _Sky.Sample(ps_SamLinear, reflectDir);
	//outps.color = testcol;
	//uint2 uv = uint2(i.uv * 128);
	////Debug[uv] = float4(reflectDir, 1.0);
	//return float4(testcol);
	////采样反射探头
	////half4 rgbm = UNITY_SAMPLE_TEXCUBE_LOD(unity_SpecCube0, reflectDir, 0);
	////half3 sky = DecodeHDR(rgbm, unity_SpecCube0_HDR);
	float4 rgbm = _Sky.Sample(ps_SamLinear, reflectDir);
	
	float3 sky = rgbm.rgb;
	
	//菲涅尔
	float fresnel = saturate(_FresnelScale + (1 - _FresnelScale) * pow(1 - dot(normal, viewDir), 5));
		  //fresnel = saturate(_FresnelScale + (1 - _FresnelScale) * pow(1 - dot(normal, viewDir), 5));
	half facing = saturate(dot(viewDir, normal));
	float3 oceanColor = lerp(_OceanColorShallow, _OceanColorDeep, facing);

	float3 ambient = float3(0.0, 0.0, 0.2);

	////泡沫颜色
	float3 bubblesDiffuse = _BubblesColor.rgb * _LightColor0.rgb * saturate(dot(lightDir, normal));
	////海洋颜色
	float3 oceanDiffuse = oceanColor * _LightColor0.rgb * saturate(dot(lightDir, normal));
	float3 halfDir = normalize(lightDir + viewDir);
	float3 specular = _LightColor0.rgb * _Specular.rgb * pow(max(0, dot(normal, halfDir)), _Gloss);

	float3 diffuse = lerp(oceanDiffuse, bubblesDiffuse, bubbles);

	float3 col = ambient + lerp(diffuse, sky, fresnel) + specular;
	//float3 col = float3(0.0,1.0,1.0);
	return float4(col, 1.0);
}