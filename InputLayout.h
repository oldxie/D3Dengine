#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* pVertexShaderBytecode);
	InputLayout(Graphics& gfx);
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {};
protected:
	ID3D11InputLayout* pInputLayout;
private:
	bool isnull = false;
};