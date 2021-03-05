#pragma once
#include "Bindable.h"
#include <map>

class StructuredBuffer : public Bindable
{
public:
	//ID3D11Device* pd3dDevice = NULL;
	ID3D11Buffer* pBuffer = NULL;
	ID3D11UnorderedAccessView* pBufUAV = NULL;
	ID3D11UnorderedAccessView* pNullBufUAV = NULL;
	ID3D11ShaderResourceView* pBufSRV = NULL;
	ID3D11ShaderResourceView* pNullBufSRV = NULL;
	std::map<LOCATION, ViewType> bindlocation;

private:
	UINT size;
public:
	StructuredBuffer(Graphics& gfx, UINT uElementSize, UINT uCount, VOID* pInitData);
	//StructuredBuffer(Graphics& gfx, ID3D11Buffer* pBuffer) :pBufferSRV(pBuffer)
	//{
	//}
	~StructuredBuffer()
	{
		//pd3dDevice->Release();
		pBuffer->Release();
		if (pBufUAV) {
			pBufUAV->Release();
		}
		if (pBufSRV)
		{
			pBufSRV->Release();
		}
	}

	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {};
	void BindtoLation(Graphics& gfx, LOCATION loc, ViewType rv) noexcept
	{
		bindlocation[loc] = rv;
	}
	static HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut);
	void BindtoCS(Graphics& gfx, ViewType rv) noexcept;
	void BindtoVS(Graphics& gfx, ViewType rv) noexcept;
	void BindtoPS(Graphics& gfx, ViewType rv) noexcept;

	void getResource(Graphics& gfx, ID3D11Buffer* buffer)
	{
		GetContext(gfx)->CopyResource(pBuffer, buffer);
	}
public:
	void CreateResourceView(Graphics& gfx, ViewType rv);

private:
	HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
	
};