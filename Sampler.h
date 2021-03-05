#pragma once
#include "Bindable.h"
class Sampler : public Bindable
{
public:
	Sampler(Graphics& gfx)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice(gfx)->CreateSamplerState(&sampDesc, &pSamplerLinear);
	}
	~Sampler() {
		pSamplerLinear->Release();
	}
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept;

	void UnBind(Graphics& gfx) noexcept
	{
		//GetContext(gfx)->VSSetSamplers(0, 1, &pSamplerLinear);
	}
protected:
	ID3D11SamplerState*                 pSamplerLinear = NULL;
};
