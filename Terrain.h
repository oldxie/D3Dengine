#pragma once
#include "Drawable.h"
#include "subWin.h"
#include <unordered_map>
/* the terrain has 3 steps to draw*/
/* 1. create a simple quad and do domain/hull shader and stream outout the geometry only
	

*/
class Terrain : public Drawable
{
	public:
		Terrain(UINT MeshSize, UINT MeshLength, Graphics& gfx);
		~Terrain();
		void Generate(Graphics& gfx);

		void Update(float dt) noexcept;
		XMMATRIX GetTransformXM() const noexcept override;
	
		//void DeferDraw(Graphics& gfx) const noexcept;
		void Draw(Graphics& gfx) const noexcept;
	public:
		int MeshSize;
		int MeshLength;
		int Totalsize;
		ID3D11Buffer* pOutputStreamBuffers = NULL;
		UINT aOffsets[1] = { 0 };
		std::vector<XMFLOAT3> PositionMap;
		std::vector<FLOAT> HeightMap;
		std::vector<XMFLOAT3> NormalMap;
		std::vector<XMFLOAT2> TextureMap;
		std::vector<WORD> VertIndexs;

private:
	struct Mapinfo
	{
		UINT16 passnum;
		ViewType vt;
	};
	//FIX ME : the resouce map
	std::unordered_map<std::unique_ptr<Bindable>, Mapinfo > ResourceMap;
	std::vector<std::unique_ptr<Bindable>> computebinds;
	std::vector<std::unique_ptr<Bindable>> heightmapbinds;
	std::unordered_map<UINT,std::vector<std::unique_ptr<Bindable>>> PassBindList;
public:
	void AddComputerBind(std::unique_ptr<Bindable> bind)noexcept;
	void AddHeightMapBind(std::unique_ptr<Bindable> bind)noexcept;
	void AddtoPassBind(std::unique_ptr<Bindable> bind, UINT passnum)noexcept;
	void AddtoResourceMap(std::unique_ptr<Bindable> bind, Mapinfo mapinfo)noexcept;

	UINT pass_num = 0;
	struct Parameters{

		float time = 0.0f;
		float height = 1.0f;
		float speed = 1.0f;
		float freq = 1.0f;
		XMFLOAT4 WindAndSeed;
	};
	Parameters parm;
	D3D11_VIEWPORT vp[2];
	ID3D11DepthStencilState* DepthStencilState = NULL;
	ID3D11DepthStencilState* DepthStencilStateHeight = NULL;
	ID3D11SamplerState*      g_pSamplerLinear = NULL;
	ID3D11Buffer* HeightBuffer;
	std::map<UINT16,std::unique_ptr<subWin>> TextureShowList;
	//std::vector<UINT> showtextureID;
};