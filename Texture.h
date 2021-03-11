#pragma once
#include "Bindable.h"
#include <map>


class Texture2D : public Bindable
{
public:
	//ID3D11Device* pd3dDevice = NULL;
	ID3D11Texture2D* pTexture = NULL;
	//ID3D11Resource* pTexture = NULL;
	ID3D11UnorderedAccessView* pUAV = NULL;
	ID3D11UnorderedAccessView* pNullUAV = NULL;
	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11ShaderResourceView* pNullSRV = NULL;
	

private:
	UINT size;
	
public:
	Texture2D(Graphics& gfx, D3D11_TEXTURE2D_DESC& desc,VOID* pInitData = nullptr);

	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override;
	std::map<LOCATION, Resourcedesc> bindlocation;
	/*void BindtoLation(Graphics& gfx, LOCATION loc, ViewType rv) noexcept
	{
		bindlocation[loc] = rv;
	}*/

	void BindtoCS(Graphics& gfx, const Resourcedesc* res) noexcept;
	void BindtoVS(Graphics& gfx, const Resourcedesc* res) noexcept;
	void BindtoPS(Graphics& gfx, const Resourcedesc* res) noexcept;

	ID3D11Texture2D* getResource()
	{
		return pTexture;
	}
public:
	void CreateResourceView(Graphics& gfx, ViewType rv);

private:
	HRESULT CreateTextureSRV(ID3D11Device* pDevice, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT CreateTextureUAV(ID3D11Device* pDevice, ID3D11UnorderedAccessView** ppUAVOut);
};