#include "Texture.h"

Texture2D::Texture2D(Graphics& gfx, D3D11_TEXTURE2D_DESC& desc, VOID* pInitData)
{
	HRESULT hr;
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = GetDevice(gfx)->CreateTexture2D(&desc, &InitData, &pTexture);
	}
	else
	{
		hr = GetDevice(gfx)->CreateTexture2D(&desc, NULL, &pTexture);
	}
	if (FAILED(hr))throw hr;
}

void Texture2D::CreateResourceView(Graphics& gfx, ViewType rv)
{
	HRESULT hr;
	switch (rv)
	{
	case ShaderResourceView:
		hr = CreateTextureSRV(GetDevice(gfx),&pSRV);
		break;
	case UnorderedAccessView:
		hr = CreateTextureUAV(GetDevice(gfx),&pUAV);
		break;
	default:
		break;
	}
	if (FAILED(hr)) throw hr;
}


HRESULT Texture2D::CreateTextureSRV(ID3D11Device* pDevice, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	//ID3D11Texture2D *pTexture2D = (ID3D11Texture2D*)pTexture;
	pTexture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;

	return pDevice->CreateShaderResourceView(pTexture, &srvDesc, ppSRVOut);
}

HRESULT Texture2D::CreateTextureUAV(ID3D11Device* pDevice, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	//ID3D11Texture2D *pTexture2D = (ID3D11Texture2D*)pTexture;
	pTexture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	//FIX ME

	return pDevice->CreateUnorderedAccessView(pTexture, &uavDesc, ppUAVOut);
}
void Texture2D::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	if (!((pSRV && (res->vt == ShaderResourceView)) || (pUAV && (res->vt == UnorderedAccessView))))
	{
		CreateResourceView(gfx, res->vt);
	}
	bindlocation[res->loc] = *res;
	switch (res->loc)
	{
	case CS:
		BindtoCS(gfx, res);
		break;
	case VS:
		BindtoVS(gfx, res);
		break;
	case PS:
		BindtoPS(gfx, res);
		break;
	default:
		break;
	}
}
void  Texture2D::BindtoCS(Graphics& gfx, const Resourcedesc* res) noexcept
{
	switch (res->vt)
	{
	case ShaderResourceView:
		GetContext(gfx)->CSSetShaderResources(res->slot, 1, &pSRV);
		break;
	case UnorderedAccessView:
		GetContext(gfx)->CSSetUnorderedAccessViews(res->slot, 1, &pUAV, NULL);
		break;
	case None:
		GetContext(gfx)->CSSetUnorderedAccessViews(res->slot, 1, &pNullUAV, NULL);
		GetContext(gfx)->CSSetShaderResources(res->slot, 1, &pNullSRV);
		break;
	default:
		break;
	}
}
void  Texture2D::BindtoVS(Graphics& gfx, const Resourcedesc* res) noexcept
{
	switch (res->vt)
	{
	case ShaderResourceView:
		GetContext(gfx)->VSSetShaderResources(res->slot, 1, &pSRV);
		break;
	case UnorderedAccessView:
		//GetContext(gfx)->VSSetUnorderedAccessViews(0, 1, &pBufUAV, NULL);
		throw "can not bind to VS by UAV";
		break;
	case None:
		GetContext(gfx)->VSSetShaderResources(res->slot, 1, &pNullSRV);
	default:
		break;
	}


}
void  Texture2D::BindtoPS(Graphics& gfx, const Resourcedesc* res) noexcept
{
	switch (res->vt)
	{
	case ShaderResourceView:
		GetContext(gfx)->PSSetShaderResources(res->slot, 1, &pSRV);
		break;
	case UnorderedAccessView:
		//GetContext(gfx)->PSSetUnorderedAccessViews(0, 1, &pBufUAV, NULL);
		throw "can not bind to PS by UAV";
		break;
	case None:
		GetContext(gfx)->PSSetShaderResources(res->slot, 1, &pNullSRV);
	default:
		break;
	}
}
 
void Texture2D::UnBind(Graphics& gfx) noexcept
{
	std::map<LOCATION, Resourcedesc>::iterator it;
	Resourcedesc res;
	res.vt = None;
	
	for (it = bindlocation.begin(); it != bindlocation.end(); ++it)
	{
		res.slot = it->second.slot;
		switch (it->first)
		{
		case CS:
			BindtoCS(gfx, &res);
			break;
		case VS:
			BindtoVS(gfx, &res);
			break;
		case PS:
			BindtoPS(gfx, &res);
			break;
		default:
			break;
		}
	}
}