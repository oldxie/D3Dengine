#include "InputLayout.h"

InputLayout::InputLayout(Graphics& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
	ID3DBlob* pVertexShaderBytecode)
{
	HRESULT hr = S_OK;
	hr = GetDevice(gfx)->CreateInputLayout(
		layout.data(), (UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	);
	if (FAILED(hr))
		throw hr;
}
InputLayout::InputLayout(Graphics& gfx)
{
	isnull = true;
}
void InputLayout::Bind(Graphics& gfx, const Resourcedesc* res ) noexcept
{
	if (isnull) GetContext(gfx)->IASetInputLayout(NULL);
	else GetContext(gfx)->IASetInputLayout(pInputLayout);
}
