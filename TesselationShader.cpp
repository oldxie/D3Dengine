#include "TesselationShader.h"

HullShader::HullShader(Graphics& gfx, WCHAR* path)
{
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "HS", "hs_5_0", &pBytecodeBlob);
	if (FAILED(hr))
		throw hr;
	//D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	hr = GetDevice(gfx)->CreateHullShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pHullShader
	);
	if (FAILED(hr))
		throw hr;
}

void HullShader::Bind(Graphics& gfx, const Resourcedesc* res ) noexcept
{
	GetContext(gfx)->HSSetShader(pHullShader, nullptr, 0u);
}
void HullShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->HSSetShader(nullptr, nullptr, 0u);
}


DomainShader::DomainShader(Graphics& gfx, WCHAR* path)
{
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "DS", "ds_5_0", &pBytecodeBlob);
	if (FAILED(hr))
		throw hr;
	//D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	hr = GetDevice(gfx)->CreateDomainShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pDomainShader
	);
	if (FAILED(hr))
		throw hr;
}

void DomainShader::Bind(Graphics& gfx, const Resourcedesc* res ) noexcept
{
	GetContext(gfx)->DSSetShader(pDomainShader, nullptr, 0u);
}
void DomainShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->DSSetShader(nullptr, nullptr, 0u);
}

