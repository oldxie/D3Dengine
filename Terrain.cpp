#include "Terrain.h"
#include "BindableBase.h"
#include "imgui/imgui.h"
#define Randmod(x) (float)(rand()%(100*x))/100.00f

Terrain::Terrain(UINT MeshSize, UINT MeshLength, Graphics& gfx)
	:
	MeshSize(MeshSize),
	MeshLength(MeshLength)
{
	Totalsize = MeshSize * MeshSize  * 6;
	Generate(gfx);
	WCHAR* PATH;
	UINT16 passnum = 0;
	//compute pass 0
	PATH = L"TerrainHigh.hlsl";
	AddResource(std::make_unique<ComputeShader>(gfx, PATH), passnum);
	UINT ParmID = AddResource(std::make_unique<ComputeConstantBuffer<Parameters>>(gfx, parm), passnum);
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

	//compute pass 1
	passnum++;
	PATH = L"FFTocean.hlsl";
	AddResource(std::make_unique<ComputeShader>(gfx, PATH, "CS"), passnum);

	ResourceDesc* RES =new ResourceDesc[passnum+1];
	for (UINT16 i = 0; i < passnum+1; i++)
	{
		RES[i] = { i,CS,UnorderedAccessView };
	}
	
	//RES[passnum + 1] = { UINT16(passnum + 1), VS, ShaderResourceView };
	

	auto Height = std::make_unique<Texture2D>(gfx, desc);
	TextureShowList.insert({ AddResourceAndDesc(std::move(Height),RES, passnum + 1), std::make_unique<subWin>(gfx, 0, 0, 128, 128) });
	delete []RES;

	//copumte pass 2
	passnum++;
	AddResource(std::make_unique<ComputeShader>(gfx, PATH, "ComputeGaussianRandom"), passnum);
	desc.Format = DXGI_FORMAT_R32G32_FLOAT;
	ResourceDesc res1[2] = {
		{ passnum,CS,UnorderedAccessView },
		{ passnum+1,CS,ShaderResourceView } //as next pass resource
	};
	TextureShowList.insert({ AddResourceAndDesc(std::make_unique<Texture2D>(gfx, desc),res1, ARRAYSIZE(res1)), std::make_unique<subWin>(gfx, 128, 0, 128, 128) });

	//copumte pass 3
	passnum++;
	AddResource(std::make_unique<ComputeShader>(gfx, PATH, "CreateHeightSpectrum"), passnum);
	ResourceDesc res2[1] = { passnum,CS,UnorderedAccessView };
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureShowList.insert({ AddResourceAndDesc(std::make_unique<Texture2D>(gfx, desc),res2, ARRAYSIZE(res2)), std::make_unique<subWin>(gfx, 256, 0, 128, 128) });
	/*ResourceDesc res3[1] = { passnum,CS,None };
	AddResourceByID(ParmID, res3);*/

	//draw pass 
	passnum++;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	PATH = L"Terrain_pass0.hlsl";
	
	AddResource(std::make_unique<PixelShader>(gfx, PATH),passnum);
	auto pvs1 = std::make_unique<VertexShader>(gfx, PATH);
	auto pvsbc1 = pvs1->GetBytecode();
	AddResource(std::move(pvs1), passnum);
	ResourceDesc res[1] = { passnum, VS, None };
	AddResourceAndDesc(std::make_unique<Sampler>(gfx), res, 1);
	AddResource(std::make_unique<InputLayout>(gfx, ied, pvsbc1), passnum);
	AddResource(std::make_unique<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), passnum);
	AddResource(std::make_unique<TransformCbuf>(gfx, *this), passnum);
	
	

	//set main vp
	vp[0].Width = 1080;
	vp[0].Height = 720;
	vp[0].MinDepth = 0.0f;
	vp[0].MaxDepth = 1.0f;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;

	//cull mode
	
	
	//add subwindow
	

}
Terrain::~Terrain()
{
}


void Terrain::Draw(Graphics& gfx) const noexcept
{
	gfx.SetCullMode(1);
	//compute
	gfx.m_pImmediateContext->RSSetViewports(1, &vp[0]);
	for (UINT16 passnum = 0; passnum < binddescs.size(); passnum++)
	{
		for (auto& desc : binddescs.at(passnum))
		{
			binds.at(desc.resindex)->Bind(gfx, &desc.res);
		}
		if (passnum < binddescs.size()-1)
		{
			gfx.Dispatch(MeshSize, MeshSize, 1);
		}
		else
		{
			gfx.SimpleDraw(Totalsize);
		}
		for (auto& desc : binddescs.at(passnum))
		{
			binds.at(desc.resindex)->UnBind(gfx);

			//temp 
			auto &c = binds.at(desc.resindex);
			if (typeid(*c) == typeid(ComputeConstantBuffer<Parameters>))
			{
				ComputeConstantBuffer<Parameters> *prt = dynamic_cast<ComputeConstantBuffer<Parameters> *> (c.get());
				prt->Update(gfx, parm);
				//prt.release();
			}
		}
	}

	if (TextureShowList.size()>0) {
		for (auto& texshow : TextureShowList)
		{
			const ResourceDesc RES[1] =
			{
				{0,PS,ShaderResourceView}
			};
			binds.at(texshow.first) -> Bind(gfx, RES);
			texshow.second->TextureDraw(gfx);
			binds.at(texshow.first)->UnBind(gfx);
		}
	}
		
	//update

	//for (auto& c : computebinds)
	//{
	//	if (typeid(*c) == typeid(Texture2D))
	//	{
	//		Texture2D *pt = dynamic_cast<Texture2D *> (c.get());
	//		//pt->CopyUAVtoSRV(gfx);
	//		pt->BindtoCS(gfx, None);
	//		pt->CreateResourceView(gfx, ShaderResourceView);
	//		pt->BindtoVS(gfx, ShaderResourceView);
	//		//c->getResource(gfx, HeightBuffer);
	//	}
	//	if (typeid(*c) == typeid(ComputeConstantBuffer<Parameters>))
	//	{
	//		ComputeConstantBuffer<Parameters> *prt = dynamic_cast<ComputeConstantBuffer<Parameters> *> (c.get());
	//		prt->Update(gfx, parm);
	//		//prt.release();
	//	}
	//}
	//
	//UINT vertexstride = sizeof(XMFLOAT3);
	//const UINT offset = 0u;
	//ID3D11Buffer* nullbuffer = nullptr;
	//for (auto it = PassBindList.begin(); it != PassBindList.end(); ++it)
	//{
	//	if (it->first == 0)
	//	{
	//		gfx.m_pImmediateContext->RSSetViewports(1, &vp[0]);
	//		gfx.m_pImmediateContext->IASetVertexBuffers(0u, 1u, &pOutputStreamBuffers, &vertexstride, &offset);
	//	}

	//	//bind
	//	for (auto& b : it->second)
	//	{
	//		b->Bind(gfx);
	//	}

	//	//draw
	//	if (it->first == 0)
	//	{
	//		//gfx.DrawAuto();
	//		//gfx.DrawIndexed(96);
	//		gfx.SimpleDraw(Totalsize);
	//	}
	//	else
	//	{
	//		gfx.SimpleDraw(Totalsize);
	//	}

	//	//reset bind
	//	for (auto& b : it->second)
	//	{
	//		b->UnBind(gfx);
	//	}
	//}
	
}
void Terrain::AddComputerBind(std::unique_ptr<Bindable> bind) noexcept
{
	computebinds.push_back(std::move(bind));
}

void Terrain::AddHeightMapBind(std::unique_ptr<Bindable> bind) noexcept
{
	heightmapbinds.push_back(std::move(bind));
}
void Terrain::AddtoPassBind(std::unique_ptr<Bindable> bind, UINT passnum) noexcept
{
	std::unordered_map<UINT,std::vector<std::unique_ptr<Bindable>>>::const_iterator got = PassBindList.find (passnum);
	if (got == PassBindList.end())
	{	
		std::vector<std::unique_ptr<Bindable>> pass;
		pass.push_back(std::move(bind));
		PassBindList.insert({passnum,std::move(pass)});
	}
	else {
		PassBindList[passnum].push_back(std::move(bind));
	}
	
}
void Terrain::AddtoResourceMap(std::unique_ptr<Bindable> bind, Mapinfo mapinfo)noexcept
{
	
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
	desc.ByteWidth = sizeof(FLOAT) * 3 * Totalsize ;
	desc.CPUAccessFlags = 0;
	//desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//desc.StructureByteStride = sizeof(FLOAT);
	gfx.m_pd3dDevice->CreateBuffer(&desc, NULL, &pOutputStreamBuffers);
	//compile the GS for os
	auto pgs = std::make_unique<GeometryShaderWithSO>(gfx, PATH);
	const D3D11_SO_DECLARATION_ENTRY pSODeclaration[1] = {
		{ 0, "SV_POSITION", 0, 0, 3, 0 },
		//{ 0, "COLOR", 0, 0, 4, 0 }
	};
	UINT vertexstride = sizeof(XMFLOAT3);
	pgs->SetParameter(gfx, &pSODeclaration[0], ARRAYSIZE(pSODeclaration), vertexstride);
	AddBind(std::move(pgs));

	AddBind(std::make_unique<VertexBuffer>(gfx, PositionMap));
	//AddBind(std::make_unique<IndexBuffer>(gfx, VertIndexs));

	auto pvs = std::make_unique<VertexShader>(gfx, PATH);
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));
	AddBind(std::make_unique<HullShader>(gfx, PATH));
	AddBind(std::make_unique<DomainShader>(gfx, PATH));
	AddBind(std::make_unique<HullConstantBuffer<UINT>>(gfx, MeshSize));
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

void Terrain::Update(float dt) noexcept
{
	static float WindScale=200;
	if (parm.time == 628)
	{
		parm.time = 0.0f;
	}
	else {
		parm.time = parm.time + 1.0f;
	}
	if (ImGui::Begin("Wave Control"))
	{
		ImGui::SliderFloat("Speed Factor", &parm.speed, 0.0f, 20.0f);
		ImGui::SliderFloat("Height Factor", &parm.height, 0.0f, 10.0f);
		ImGui::SliderFloat("Freq Factor", &parm.freq, 0.0f, 2.0f);
		ImGui::SliderFloat("WindScale", &WindScale, 0.0f, 400.0f);
	}
	ImGui::End();
	XMFLOAT4 WindAndSeed = XMFLOAT4(0.1f, 0.2f, 0, 0);
	WindAndSeed.z = Randmod(10);
	WindAndSeed.w = Randmod(10);
	XMFLOAT2 wind;
	XMStoreFloat2(&wind, XMVector2Normalize(XMLoadFloat2(&XMFLOAT2(WindAndSeed.x, WindAndSeed.y))) *= WindScale);
	parm.WindAndSeed=XMFLOAT4(wind.x, wind.y, WindAndSeed.z, WindAndSeed.w);
}

XMMATRIX Terrain::GetTransformXM() const noexcept
{
	return XMMatrixIdentity();
}