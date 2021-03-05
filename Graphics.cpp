#pragma once
#include "Graphics.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Graphics::Graphics(HWND hwnd)
	
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		throw "create pBackBuffer fail ! ";

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		throw "create m_pRenderTargetView fail ! ";

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width ;
	descDepth.Height = height ;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//depth 24 bits, stencil 8 bits
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
	if (FAILED(hr))
		throw "create m_pDepthStencil fail";

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		throw "create m_pDepthStencilView fail";

	
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	camera = new Camera();
	ImGui_ImplDX11_Init(m_pd3dDevice, m_pImmediateContext);
}
void Graphics::SetCullMode(CullType CullType)
{
	ID3D11RasterizerState* pRSState;
	D3D11_RASTERIZER_DESC rd = {};
	if (CullType == 1)
	{
		rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	}
	else if (CullType == 0)
	{
		rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	}
	//m_pd3dDevice->CreateRasterizerState(&rd, &pRSState);
	m_pd3dDevice->CreateRasterizerState(&rd, &pRSState);
	m_pImmediateContext->RSSetState(pRSState);
	
}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = m_pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw m_pd3dDevice->GetDeviceRemovedReason();
		}
		else
		{
			throw hr;
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH| D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

//draw
void Graphics::DrawIndexed(UINT count) noexcept
{
	m_pImmediateContext->DrawIndexed(count, 0u, 0u);
}
void Graphics::DrawAuto() noexcept
{
	m_pImmediateContext->DrawAuto();
}

void Graphics::DrawIndexedToVP(UINT count, UINT vp_index) noexcept
{
	
}

void Graphics::SimpleDraw(UINT num) noexcept
{
	m_pImmediateContext->Draw(num, 0u);
}
//dispatch
void Graphics::Dispatch(UINT elements_x, UINT elements_y, UINT elements_z) noexcept
{
	m_pImmediateContext->Dispatch(elements_x, elements_y, elements_z);
}




void Graphics::SetProjection(XMMATRIX proj) noexcept
{
	projection = proj;
}

XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

Camera* Graphics::GetCamera() const noexcept
{
	return camera;
}
void Graphics::SetCamera() const noexcept
{
}

Graphics::~Graphics()
{
	/*m_pd3dDevice->Release();
	m_pImmediateContext->Release();
	m_pSwapChain->Release();
	m_pRenderTargetView->Release();
	m_pDepthStencil->Release();
	m_pDepthStencilView->Release();
	ppDepthStencilState->Release();*/
}