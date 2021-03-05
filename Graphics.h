#pragma once
#include <windows.h>
#include <stdio.h>
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <memory>
#include <vector>
#include "resource.h"
#include <xnamath.h>
#include "Camera.h"
typedef UINT8 CullType;

class Graphics
{
public:
	D3D_DRIVER_TYPE         m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           m_pd3dDevice = NULL;
	ID3D11DeviceContext*    m_pImmediateContext = NULL;
	IDXGISwapChain*         m_pSwapChain = NULL;
	ID3D11RenderTargetView* m_pRenderTargetView = NULL;
	ID3D11Texture2D*        m_pDepthStencil = NULL;
	ID3D11DepthStencilView* m_pDepthStencilView = NULL;
	ID3D11DepthStencilState* ppDepthStencilState = NULL;
	ID3D11DepthStencilState* ppDepthStencilState2 = NULL;	

private:
	XMMATRIX projection;
	Camera* camera;
public:
	Graphics(HWND hwnd);

	void SetCullMode(CullType CullType);
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noexcept;
	void DrawAuto() noexcept;
	void SimpleDraw(UINT num) noexcept;
	void Dispatch(UINT elements_x, UINT elements_y, UINT elements_z) noexcept;
	void DrawIndexedToVP(UINT count,UINT vp_index) noexcept;
	void SetProjection(XMMATRIX proj) noexcept;
	XMMATRIX GetProjection() const noexcept;
	Camera* GetCamera() const noexcept;
	void SetCamera() const noexcept;
	
};
