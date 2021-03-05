#pragma once
#include "Bindable.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& consts)
	{

		D3D11_MAPPED_SUBRESOURCE msr;
		GetContext(gfx)->Map(
			pConstantBuffer, 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);//msr ,cpu mem point to the gpu mem
		memcpy(msr.pData, &consts, sizeof(consts));
		GetContext(gfx)->Unmap(pConstantBuffer, 0u);
	}
	ConstantBuffer(Graphics& gfx, const C& consts)
	{
		//INFOMAN(gfx);
		UINT size = sizeof(consts);
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = size*4;
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer);

		//son = *this;
	}
	ConstantBuffer(Graphics& gfx)
	{

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;
		GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer);
	}
protected:
	ID3D11Buffer* pConstantBuffer;
	void UnBind(Graphics& gfx) noexcept override
	{
	}
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	}
};

template<typename C>
class HullConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override
	{
		GetContext(gfx)->HSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	}
};

template<typename C>
class DomainConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override
	{
		GetContext(gfx)->DSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	}
};

template<typename C>
class ComputeConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override
	{
		GetContext(gfx)->CSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	}
	
};