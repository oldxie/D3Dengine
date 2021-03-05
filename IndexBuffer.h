#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer( Graphics& gfx,const std::vector<unsigned short>& indices );
	void Bind( Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {};
	UINT GetCount() const noexcept;
protected:
	UINT count;
	ID3D11Buffer* pIndexBuffer;
};