#pragma once
#include "Bindable.h"
#include <map>
typedef UINT8 ViewType;
enum Viewtype
{
	ShaderResourceView,
	UnorderedAccessView,
	None,
};
typedef UINT8 LOCATION;
enum location
{
	VS,
	PS,
	CS
};

class ComputeBuffers : public Bindable
{
public:
	//ID3D11Device* pd3dDevice = NULL;
	ID3D11Buffer* pBufferUAV = NULL;
	ID3D11Buffer* pBufferSRV = NULL;
	ID3D11Texture2D* pTexture2D = NULL;
	ID3D11UnorderedAccessView* pBufUAV = NULL;
	ID3D11UnorderedAccessView* pNullBufUAV = NULL;
	ID3D11ShaderResourceView* pBufSRV = NULL;
	ID3D11ShaderResourceView* pNullBufSRV = NULL;
	std::map<LOCATION, ViewType> bindlocation;

private:
	UINT size;
public:
	ComputeBuffers(Graphics& gfx , UINT uElementSize, UINT uCount, VOID* pInitData);
	ComputeBuffers(Graphics& gfx, ID3D11Buffer* pBuffer):pBufferSRV(pBuffer)
	{
	}
	~ComputeBuffers()
	{
		//pd3dDevice->Release();
		if (pBufferUAV) {
			pBufferUAV->Release();
			pBufUAV->Release();
		}
		if (pBufferSRV)
		{
			pBufferSRV->Release();
			pBufSRV->Release();
		}
	}
	
	void Bind(Graphics& gfx) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {};
	void BindtoLation(Graphics& gfx, LOCATION loc, ViewType rv) noexcept
	{
		bindlocation[loc] = rv;
	}
	static HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut);
	static HRESULT CreateTexture2D(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Texture2D** pTexture2D);
	void BindtoCS(Graphics& gfx, ViewType rv) noexcept;
	void BindtoVS(Graphics& gfx, ViewType rv) noexcept;
	void BindtoPS(Graphics& gfx, ViewType rv) noexcept;
	void CopyUAVtoSRV(Graphics& gfx)
	{
		//GetContext(gfx)->CopyResource(pBufferSRV, pBufferUAV);
		verify(gfx);
	}
	void getResource(Graphics& gfx, ID3D11Buffer* buffer)
	{
		GetContext(gfx)->CopyResource(pBufferUAV, buffer);
	}
public:
	void CreateResourceView(Graphics& gfx, ViewType rv);
	
private:
	HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
	;

public:

	bool verify(Graphics& gfx)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(GetDevice(gfx), GetContext(gfx), pBufferUAV);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		float *p;
		GetContext(gfx)->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
		// This is also a common trick to debug CS programs.
		p = (float*)MappedResource.pData;

		//g_pImmediateContext->UpdateSubresource(g_pVertexBuffer, 0, NULL, p, 0, 0);


		// Verify that if Compute Shader has done right
		//printf("Verifying against CPU result...");
		BOOL bSuccess = TRUE;
		float r;
		for (int i = 0; i < size; ++i)
		{
			 r = p[i];
		}
		return bSuccess;
	}

private:
	ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
	{
		ID3D11Buffer* debugbuf = NULL;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		pBuffer->GetDesc(&desc);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		if (SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)))
		{
	#if defined(DEBUG) || defined(PROFILE)
			debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
	#endif

			pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
		}

		return debugbuf;
	}
};

