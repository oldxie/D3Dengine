#include "PixelShader.h"

PixelShader::PixelShader(Graphics& gfx, WCHAR* path)
{
	ID3DBlob* pBlob;
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "PS", "ps_4_0", &pBlob);
	if (FAILED(hr))
		throw hr;
	hr = GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);\
	if (FAILED(hr))
		throw hr;

}

void PixelShader::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader, nullptr, 0u);
}
void PixelShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
}
