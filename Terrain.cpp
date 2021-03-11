#include "Terrain.h"
#include "BindableBase.h"
#include "imgui/imgui.h"
#define Randmod(x) (float)(rand()%(100*x))/100.00f

Terrain::Terrain(UINT MeshSize, UINT MeshLength, Graphics& gfx)
	:
	MeshSize(MeshSize),
	MeshLength(MeshLength)
{
	Totalsize = 64 * 64  * 6;
	Generate(gfx);
	WCHAR* PATH;

	//compute pass 0
	PATH = L"TerrainHigh.hlsl";
	CreateResource("sinshader" , std::make_unique<ComputeShader>(gfx, PATH));
	CreateResource("constparm" , std::make_unique<ComputeConstantBuffer<Parameters>>(gfx, parm));
	CreateResource("windowparm", std::make_unique<PixelConstantBuffer<Windowparm>>(gfx, windowparm));

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = MeshSize;
	desc.Height = MeshSize;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	CreateResource("sinheight", std::make_unique<Texture2D>(gfx, desc));
	//TextureShowList.insert({ "sinheight", std::make_unique<subWin>(gfx, 0, 0, 128, 128) });

	PATH = L"FFTocean.hlsl";
	CreateResource("ComputeGaussian",std::make_unique<ComputeShader>(gfx, PATH, "ComputeGaussianRandom"));
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CreateResource("GaussianRandomRT",std::make_unique<Texture2D>(gfx, desc));
	//TextureShowList.insert({ "GaussianRandomRT", std::make_unique<subWin>(gfx, 128, 0, 128, 128) });

	CreateResource("ComputeHeightSpectrum",std::make_unique<ComputeShader>(gfx, PATH, "CreateHeightSpectrum"));
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CreateResource("HeightSpectrumRT",std::make_unique<Texture2D>(gfx, desc));
	TextureShowList.insert({ "HeightSpectrumRT", std::make_unique<subWin>(gfx, 0, 0, 128, 128) });
	
	CreateResource("ComputeFFTHori", std::make_unique<ComputeShader>(gfx, PATH, "FFTHorizontal"));
	CreateResource("ComputeFFTHoriEnd", std::make_unique<ComputeShader>(gfx, PATH, "FFTHorizontalEnd"));
	CreateResource("ComputeFFTVert", std::make_unique<ComputeShader>(gfx, PATH, "FFTVertical"));
	CreateResource("ComputeFFTVertEnd", std::make_unique<ComputeShader>(gfx, PATH, "FFTVerticalEnd"));

	CreateResource("InFFT", std::make_unique<Texture2D>(gfx, desc));
	CreateResource("OutFFT", std::make_unique<Texture2D>(gfx, desc));
	CreateResource("OutFFT0", std::make_unique<Texture2D>(gfx, desc));
	CreateResource("OutFFT1", std::make_unique<Texture2D>(gfx, desc));
	CreateResource("OutFFT2", std::make_unique<Texture2D>(gfx, desc));
	

	CreateResource("TempRT", nullptr);

	CreateResource("CreateDisplaceSpectrum", std::make_unique<ComputeShader>(gfx, PATH, "CreateDisplaceSpectrum"));
	CreateResource("DisplaceXSpectrumRT", std::make_unique<Texture2D>(gfx, desc));
	TextureShowList.insert({ "DisplaceXSpectrumRT", std::make_unique<subWin>(gfx, 128, 0, 128, 128) });
	CreateResource("DisplaceZSpectrumRT", std::make_unique<Texture2D>(gfx, desc));
	TextureShowList.insert({ "DisplaceZSpectrumRT", std::make_unique<subWin>(gfx, 256, 0, 128, 128) });
	CreateResource("DisplaceRT", std::make_unique<Texture2D>(gfx, desc));
	TextureShowList.insert({ "DisplaceRT", std::make_unique<subWin>(gfx, 384, 0, 128, 128) });

	CreateResource("TextureGenerationDisplace", std::make_unique<ComputeShader>(gfx, PATH, "TextureGenerationDisplace"));

	CreateResource("TextureGenerationNormalBubbles", std::make_unique<ComputeShader>(gfx, PATH, "TextureGenerationNormalBubbles"));
	CreateResource("NormalRT", std::make_unique<Texture2D>(gfx, desc));
	CreateResource("BubblesRT", std::make_unique<Texture2D>(gfx, desc));
	TextureShowList.insert({ "NormalRT", std::make_unique<subWin>(gfx, 512, 0, 128, 128) });
	TextureShowList.insert({ "BubblesRT", std::make_unique<subWin>(gfx, 640, 0, 128, 128) });
	//TextureShowList.insert({ "OutFFT", std::make_unique<subWin>(gfx, 384, 0, 128, 128) });
	//////draw pass 

	PATH = L"Terrain_pass0.hlsl";
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	auto pvs1 = std::make_unique<VertexShader>(gfx, PATH);
	auto pvsbc1 = pvs1->GetBytecode();
	CreateResource("CameraMartrix",std::make_unique<TransformCbuf>(gfx, *this));
	CreateResource("DrawVertexShader",std::move(pvs1));
	CreateResource("Layout", std::make_unique<InputLayout>(gfx, ied, pvsbc1));
	CreateResource("DrawPixelShader", std::make_unique<PixelShader>(gfx, PATH));
	CreateResource("Topolog",std::make_unique<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	CreateResource("SamplerVS", std::make_unique<Sampler>(gfx));
	CreateResource("SamplerPS", std::make_unique<Sampler>(gfx));
	
	SetResource(gfx, "ComputeGaussian");
	SetResource(gfx, "GaussianRandomRT", CS, UnorderedAccessView, 1);
	Dispatch(gfx, MeshSize / 8, MeshSize / 8, 1);
	

	//temperary
	HRESULT hr = S_OK;
	hr = D3DX11CreateShaderResourceViewFromFile(gfx.m_pd3dDevice, L"sunsetcube1024.dds", NULL, NULL, &g_sky, NULL);
	if (FAILED(hr))
		throw hr;
}
Terrain::~Terrain()
{
}
void Terrain::computeFFT(Graphics& gfx ,LPCSTR kernel, LPCSTR input)
{
	SetResource(gfx, input, CS, ShaderResourceView,6);
	SetResource(gfx, "OutFFT", CS, UnorderedAccessView,6);
	SetResource(gfx, kernel);
	
	Dispatch(gfx, MeshSize / 8, MeshSize / 8, 1);
	resoucemap.at("TempRT") = std::move(resoucemap.at(input));
	resoucemap.at(input) = std::move(resoucemap.at("OutFFT"));
	resoucemap.at("OutFFT") = std::move(resoucemap.at("TempRT"));
}
void Terrain::Update(float dt, Graphics& gfx) noexcept
{
	static float WindScale = 200;

	/*if (parm.time == 628)
	{
		parm.time = 0.0f;
	}
	else {*/
		parm.time = parm.time + 0.04f;
	//}
	if (ImGui::Begin("Wave Control"))
	{
		ImGui::SliderFloat("Speed Factor", &parm.speed, 0.0f, 20.0f);
		ImGui::SliderFloat("Height Factor", &parm.height, 0.0f, 10.0f);
		ImGui::SliderFloat("Freq Factor", &parm.freq, 0.0f, 2.0f);
		ImGui::SliderFloat("WindScale", &WindScale, 0.0f, 400.0f);
		ImGui::SliderFloat("BubblesScale", &parm.BubblesScale, 0.0f, 1.0f);
		ImGui::SliderFloat("BubblesThreshold", &parm.BubblesThreshold, 0.0f, 1.0f);
	}
	ImGui::End();

	if (ImGui::Begin("Showtexture Control"))
	{
	ImGui::SliderFloat("light", &windowparm.colormax, 0.0f, 1.0f);
	ImGui::SliderFloat("duibi", &windowparm.colorscale, 0.0f, 100.0f);
	}
	ImGui::End();
	XMFLOAT4 WindAndSeed = XMFLOAT4(0.1f, 0.2f, 0, 0);
	WindAndSeed.z = Randmod(10);
	WindAndSeed.w = Randmod(10);
	XMFLOAT2 wind;
	XMStoreFloat2(&wind, XMVector2Normalize(XMLoadFloat2(&XMFLOAT2(WindAndSeed.x, WindAndSeed.y))) *= WindScale);
	parm.WindAndSeed = XMFLOAT4(wind.x, wind.y, WindAndSeed.z, WindAndSeed.w);

	ComputeConstantBuffer<Parameters> *prt = dynamic_cast<ComputeConstantBuffer<Parameters> *> (resoucemap.at("constparm").get());
	prt->Update(gfx, parm);
	PixelConstantBuffer<Windowparm> *prt2 = dynamic_cast<PixelConstantBuffer<Windowparm> *> (resoucemap.at("windowparm").get());
	prt2->Update(gfx, windowparm);

	/*SetResource(gfx, "sinshader");
	
	SetResource(gfx, "sinheight", CS, UnorderedAccessView);

	Dispatch(gfx, MeshSize /8, MeshSize /8, 1);*/
	SetResource(gfx, "constparm");
	SetResource(gfx, "ComputeHeightSpectrum");
	SetResource(gfx, "GaussianRandomRT", CS, ShaderResourceView,0);
	SetResource(gfx, "HeightSpectrumRT", CS, UnorderedAccessView,2);
	Dispatch(gfx, MeshSize /8, MeshSize /8, 1);

	
	SetResource(gfx, "CreateDisplaceSpectrum");
	SetResource(gfx, "HeightSpectrumRT", CS, ShaderResourceView,1);
	SetResource(gfx, "DisplaceXSpectrumRT", CS, UnorderedAccessView,3);
	SetResource(gfx, "DisplaceZSpectrumRT", CS, UnorderedAccessView,4);


	Dispatch(gfx, MeshSize / 8, MeshSize / 8, 1);

	
	//Hori FFT
	for (int i = 1; i <= 7  ; i++)
	{
		parm.Ns = pow(2, i-1);
		prt->Update(gfx, parm);
		if (i == 7)
		{
			computeFFT(gfx, "ComputeFFTHoriEnd", "HeightSpectrumRT");
			computeFFT(gfx, "ComputeFFTHoriEnd", "DisplaceXSpectrumRT");
			computeFFT(gfx, "ComputeFFTHoriEnd", "DisplaceZSpectrumRT");

		}
		else
		{
			computeFFT(gfx, "ComputeFFTHori", "HeightSpectrumRT");
			computeFFT(gfx, "ComputeFFTHori", "DisplaceXSpectrumRT");
			computeFFT(gfx, "ComputeFFTHori", "DisplaceZSpectrumRT");
		}
		
		//the final output is HeightSpectrumRT
	}

	//Vert FFT
	for (int i = 1; i <=7  ; i++)
	{
		parm.Ns = pow(2, i-1);
		prt->Update(gfx, parm);
		if (i == 7)
		{
			computeFFT(gfx, "ComputeFFTVertEnd", "HeightSpectrumRT");
			computeFFT(gfx, "ComputeFFTVertEnd", "DisplaceXSpectrumRT");
			computeFFT(gfx, "ComputeFFTVertEnd", "DisplaceZSpectrumRT");
			
		}
		else
		{
			computeFFT(gfx, "ComputeFFTVert", "HeightSpectrumRT");
			computeFFT(gfx, "ComputeFFTVert", "DisplaceXSpectrumRT");
			computeFFT(gfx, "ComputeFFTVert", "DisplaceZSpectrumRT");
		}
		//the final output is HeightSpectrumRT
	}
	

	

	SetResource(gfx, "TextureGenerationDisplace");
	SetResource(gfx, "HeightSpectrumRT", CS, ShaderResourceView, 1);
	SetResource(gfx, "DisplaceXSpectrumRT", CS, ShaderResourceView, 2);
	SetResource(gfx, "DisplaceZSpectrumRT", CS, ShaderResourceView, 3);

	SetResource(gfx, "OutFFT0", CS, UnorderedAccessView, 2);
	SetResource(gfx, "OutFFT1", CS, UnorderedAccessView, 3);
	SetResource(gfx, "OutFFT2", CS, UnorderedAccessView, 4);
	SetResource(gfx, "DisplaceRT", CS, UnorderedAccessView, 5);
	Dispatch(gfx, MeshSize / 8, MeshSize / 8, 1);

	resoucemap.at("TempRT") = std::move(resoucemap.at("HeightSpectrumRT"));
	resoucemap.at("HeightSpectrumRT") = std::move(resoucemap.at("OutFFT0"));
	resoucemap.at("OutFFT0") = std::move(resoucemap.at("TempRT"));

	resoucemap.at("TempRT") = std::move(resoucemap.at("DisplaceXSpectrumRT"));
	resoucemap.at("DisplaceXSpectrumRT") = std::move(resoucemap.at("OutFFT1"));
	resoucemap.at("OutFFT1") = std::move(resoucemap.at("TempRT"));

	resoucemap.at("TempRT") = std::move(resoucemap.at("DisplaceZSpectrumRT"));
	resoucemap.at("DisplaceZSpectrumRT") = std::move(resoucemap.at("OutFFT2"));
	resoucemap.at("OutFFT2") = std::move(resoucemap.at("TempRT"));

	//generate bubbles and normal texture
	
	SetResource(gfx, "TextureGenerationNormalBubbles");
	SetResource(gfx, "DisplaceRT", CS, ShaderResourceView, 4);
	SetResource(gfx, "NormalRT", CS, UnorderedAccessView, 6);
	SetResource(gfx, "BubblesRT", CS, UnorderedAccessView, 7);

	Dispatch(gfx, MeshSize / 8, MeshSize / 8, 1);

	SetResource(gfx, "windowparm");
	
	ShowTexture(gfx);
	//draw

	SetResource(gfx, "DrawVertexShader");
	SetResource(gfx, "DrawPixelShader");
	SetResource(gfx, "CameraMartrix");
	SetResource(gfx, "Layout");
	SetResource(gfx, "Topolog");

	SetResource(gfx, "NormalRT", PS, ShaderResourceView, 0);
	SetResource(gfx, "BubblesRT", PS, ShaderResourceView, 1);
	SetResource(gfx, "DisplaceRT", VS, ShaderResourceView);
	SetResource(gfx, "SamplerVS", VS);
	SetResource(gfx, "SamplerPS", PS);
	gfx.m_pImmediateContext->PSSetShaderResources(2, 1, &g_sky);
	//set main vp
	vp[0].Width = 1080;
	vp[0].Height = 720;
	vp[0].MinDepth = 0.0f;
	vp[0].MaxDepth = 1.0f;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;

	Draw(gfx, draw, vp, Totalsize, 0);
}

void Terrain::ShowTexture(Graphics& gfx) const noexcept
{
	if (TextureShowList.size()>0) {
		for (auto& texshow : TextureShowList)
		{
			const ResourceDesc RES[1] =
			{
				{0,PS,ShaderResourceView}
			};
			resoucemap.at(texshow.first)-> Bind(gfx, RES);
			texshow.second->TextureDraw(gfx);
			resoucemap.at(texshow.first)->UnBind(gfx);
		}
	}
		
}

void Terrain::Generate(Graphics& gfx)
{
	int index_size = (MeshSize - 1) * (MeshSize - 1) * 6;
	XMFLOAT3 Pos;
	//XMFLOAT3 HeightViewPos;
	XMFLOAT2 uv;
	XMFLOAT3 Normal;
	int inx = 0;

	// generate by cpu
	//for (int i = 0; i < MeshSize; i++)
	//{
	//	for (int j = 0; j < MeshSize; j++)
	//	{
	//		int index = i * MeshSize + j;
	//		Pos = XMFLOAT3((j - MeshSize / 2.0f) * MeshLength / MeshSize, 0, (i - MeshSize / 2.0f) * MeshLength / MeshSize);
	//		uv = XMFLOAT2(2*j / (MeshSize - 1.0f)-1.0f, 2*i / (MeshSize - 1.0f)-1.0f);
	//		//HeightViewPos = XMFLOAT3(j / (MeshSize - 1.0f), i / (MeshSize - 1.0f),0);
	//		Normal = XMFLOAT3(0, 1, 0);
	//		PositionMap.push_back(Pos);
	//		TextureMap.push_back(uv);
	//		NormalMap.push_back(Normal);
	//		HeightMap.push_back(1.0);
	//		/*if (i != MeshSize - 1 && j != MeshSize - 1)
	//		{
	//			VertIndexs.push_back(index);
	//			VertIndexs.push_back(index + MeshSize);
	//			VertIndexs.push_back(index + MeshSize + 1);

	//			VertIndexs.push_back(index);
	//			VertIndexs.push_back(index + MeshSize + 1);
	//			VertIndexs.push_back(index + 1);
	//		}*/
	//		if (i != MeshSize - 1 && j != MeshSize - 1)
	//		{
	//			VertIndexs.push_back(index + 1);
	//			VertIndexs.push_back(index );
	//			VertIndexs.push_back(index + MeshSize + 1);
	//			VertIndexs.push_back(index + MeshSize );
	//			
	//		}
	//	}
	//}

	//generate by GPU outputstream
	const UINT offset = 0u;
	ID3D11Buffer* nullbuffer = nullptr;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Pos = XMFLOAT3((i - 1 / 2.0f) * MeshLength, 0, (j - 1 / 2.0f) * MeshLength);
			PositionMap.push_back(Pos);
		}
	}
	WCHAR* PATH = L"generate.hlsl";
	//create a buffer for os
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	desc.ByteWidth = sizeof(FLOAT) * 3 * 64 * 64*6 ;
	desc.CPUAccessFlags = 0;
	//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//desc.StructureByteStride = sizeof(FLOAT);
	gfx.m_pd3dDevice->CreateBuffer(&desc, NULL, &pOutputStreamBuffers);
	//compile the GS for os
	auto pgs = std::make_unique<GeometryShaderWithSO>(gfx, PATH);
	const D3D11_SO_DECLARATION_ENTRY pSODeclaration[1] = {
		{ 0, "SV_POSITION", 0, 0, 3, 0 },
		//{ 0, "TEXCOORD", 0, 0, 2, 0 }
	};
	UINT vertexstride = sizeof(XMFLOAT3) ;
	pgs->SetParameter(gfx, &pSODeclaration[0], ARRAYSIZE(pSODeclaration), vertexstride);
	AddBind(std::move(pgs));

	AddBind(std::make_unique<VertexBuffer>(gfx, PositionMap));
	//AddBind(std::make_unique<IndexBuffer>(gfx, VertIndexs));

	auto pvs = std::make_unique<VertexShader>(gfx, PATH);
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));
	AddBind(std::make_unique<HullShader>(gfx, PATH));
	AddBind(std::make_unique<DomainShader>(gfx, PATH));
	AddBind(std::make_unique<HullConstantBuffer<UINT>>(gfx, 64));
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST));


	//kick-off a OS pipeline
	
	gfx.m_pImmediateContext->SOSetTargets(1, &nullbuffer, &offset);
	gfx.m_pImmediateContext->IASetInputLayout(nullptr);
	gfx.m_pImmediateContext->SOSetTargets(0, nullptr, &offset);
	gfx.m_pImmediateContext->RSSetState(nullptr);
	gfx.m_pImmediateContext->SOSetTargets(1, &pOutputStreamBuffers, &offset);
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.SimpleDraw(4);
	for (auto& b : binds)
	{
		b->UnBind(gfx);
	}
	gfx.m_pImmediateContext->SOSetTargets(1, &nullbuffer, 0);
	gfx.m_pImmediateContext->IASetVertexBuffers(0u, 1u, &pOutputStreamBuffers, &vertexstride, &offset);


}



XMMATRIX Terrain::GetTransformXM() const noexcept
{
	return XMMatrixIdentity();
}