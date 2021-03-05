#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer(Graphics& gfx, UINT uElementSize, UINT uCount, VOID* pInitData)
	:size(uCount)
{
	CreateStructuredBuffer(GetDevice(gfx), uElementSize, uCount, NULL, &pBuffer);
}
void StructuredBuffer::CreateResourceView(Graphics& gfx, ViewType rv)
{
	switch (rv)
	{
	case ShaderResourceView:
		CreateBufferSRV(GetDevice(gfx), pBuffer, &pBufSRV);
		break;
	case UnorderedAccessView:
		CreateBufferUAV(GetDevice(gfx), pBuffer, &pBufUAV);
		break;
	default:
		break;
	}
}

HRESULT StructuredBuffer::CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufOut);
}

HRESULT StructuredBuffer::CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

HRESULT StructuredBuffer::CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}
void StructuredBuffer::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	std::map<LOCATION, ViewType>::iterator it;
	for (it = bindlocation.begin(); it != bindlocation.end(); ++it)
	{
		CreateResourceView(gfx, it->second);
		switch (it->first)
		{
		case CS:
			BindtoCS(gfx, it->second);
			break;
		case VS:
			BindtoVS(gfx, it->second);
			break;
		case PS:
			BindtoPS(gfx, it->second);
			break;
		default:
			break;
		}
	}
}
void  StructuredBuffer::BindtoCS(Graphics& gfx, ViewType rv) noexcept
{
	switch (rv)
	{
	case ShaderResourceView:
		GetContext(gfx)->CSSetShaderResources(0, 1, &pBufSRV);
		break;
	case UnorderedAccessView:
		GetContext(gfx)->CSSetUnorderedAccessViews(0, 1, &pBufUAV, NULL);
		break;
	case None:
		GetContext(gfx)->CSSetUnorderedAccessViews(0, 1, &pNullBufUAV, NULL);
		GetContext(gfx)->CSSetShaderResources(0, 1, &pNullBufSRV);
		break;
	default:
		break;
	}
}
void  StructuredBuffer::BindtoVS(Graphics& gfx, ViewType rv) noexcept
{
	switch (rv)
	{
	case ShaderResourceView:
		GetContext(gfx)->VSSetShaderResources(0, 1, &pBufSRV);
		break;
	case UnorderedAccessView:
		//GetContext(gfx)->VSSetUnorderedAccessViews(0, 1, &pBufUAV, NULL);
		throw "can not bind to VS by UAV";
		break;
	case None:
		GetContext(gfx)->VSSetShaderResources(0, 1, &pNullBufSRV);
	default:
		break;
	}


}
void  StructuredBuffer::BindtoPS(Graphics& gfx, ViewType rv) noexcept
{
	switch (rv)
	{
	case ShaderResourceView:
		GetContext(gfx)->PSSetShaderResources(0, 1, &pBufSRV);
		break;
	case UnorderedAccessView:
		//GetContext(gfx)->PSSetUnorderedAccessViews(0, 1, &pBufUAV, NULL);
		throw "can not bind to PS by UAV";
		break;
	default:
		break;
	}
}
