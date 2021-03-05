#include "GeometryShader.h"

GeometryShader::GeometryShader(Graphics& gfx, WCHAR* path)
{
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "GS", "gs_4_0", &pBytecodeBlob);
	if (FAILED(hr))
		throw hr;
	//D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	hr = GetDevice(gfx)->CreateGeometryShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pGeometryShader
	);
	if (FAILED(hr))
		throw hr;
}

void GeometryShader::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->GSSetShader(pGeometryShader, nullptr, 0u);
}
void GeometryShader::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->GSSetShader(nullptr, nullptr, 0u);
}

ID3DBlob* GeometryShader::GetBytecode() const noexcept
{
	return pBytecodeBlob;
}


//streamout
GeometryShaderWithSO::GeometryShaderWithSO(Graphics& gfx, WCHAR* path)
{
	HRESULT hr = S_OK;
	hr = CompileShaderFromFile(path, "DS", "ds_5_0", &pBytecodeBlob);
	if (FAILED(hr))
		throw hr;
}
void GeometryShaderWithSO::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->GSSetShader(pGeometryShader, nullptr, 0u);
}
void GeometryShaderWithSO::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->GSSetShader(nullptr, nullptr, 0u);
}
void GeometryShaderWithSO::SetParameter(Graphics& gfx,const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, UINT pBufferStrides)
{
	    gfx.m_pd3dDevice->CreateGeometryShaderWithStreamOutput(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		pSODeclaration,
		NumEntries,
		&pBufferStrides,
		1,
		D3D11_SO_NO_RASTERIZED_STREAM, nullptr,
		&pGeometryShader
	);
}
/*
HRESULT CreateGeometryShaderWithStreamOutput(
	[in]   const void *pShaderBytecode,
	[in]   SIZE_T BytecodeLength,
	[in]   const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
	[in]   UINT NumEntries,
	[in]   const UINT *pBufferStrides,
	[in]   UINT NumStrides,
	[in]   UINT RasterizedStream,
	[in]   ID3D11ClassLinkage *pClassLinkage,
	[out]  ID3D11GeometryShader **ppGeometryShader
);
*/