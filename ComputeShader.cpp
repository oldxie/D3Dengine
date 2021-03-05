#include "ComputeShader.h"

ComputeShader::ComputeShader(Graphics& gfx, WCHAR* path, const LPCSTR FunctionName)
{
	ID3DBlob* pBlob;
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, FunctionName, "cs_5_0", &pBlob);
	if (FAILED(hr))
		throw hr;
	hr = GetDevice(gfx)->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pComputeShader); 
		if (FAILED(hr))
			throw hr;

}

void ComputeShader::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->CSSetShader(pComputeShader, nullptr, 0u);
}
void ComputeShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->CSSetShader(nullptr, nullptr, 0u);
}