#include "VertexShader.h"

VertexShader::VertexShader(Graphics& gfx, WCHAR* path)
{
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "VS", "vs_5_0", &pBytecodeBlob);
	if (FAILED(hr))
		throw hr;
	//D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	hr = GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	);
	if (FAILED(hr))
		throw hr;
}

void VertexShader::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader, nullptr, 0u);
}
void VertexShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(nullptr, nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob;
}