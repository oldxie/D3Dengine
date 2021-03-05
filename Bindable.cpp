#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
{
	return gfx.m_pImmediateContext;
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
	return gfx.m_pd3dDevice;
}

HRESULT Bindable::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	const D3D_SHADER_MACRO defines[] =
	{
		"USE_STRUCTURED_BUFFERS", "1",
		NULL, NULL
	};
	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, defines, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}