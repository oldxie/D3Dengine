#pragma once
#include "Bindable.h"


class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
		:
		stride(sizeof(V))
	{
		//INFOMAN(gfx);
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		bd.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();	
		hr = GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer);
		if (FAILED(hr))
			throw hr;
	}
	/*VertexBuffer(Graphics& gfx, ID3D11Buffer* ibuf)
	{
		pVertexBuffer = ibuf;
	}*/
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {};
protected:
	UINT stride;
	ID3D11Buffer* pVertexBuffer = NULL;
};

