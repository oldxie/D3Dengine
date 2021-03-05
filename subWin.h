#pragma once
#include "Drawable.h"
#include "BindableBase.h"
#include "Bindable.h"
class subWin : public Drawable
{
public:
	subWin(Graphics& gfx, FLOAT topleftx, FLOAT toplefty ,FLOAT width, FLOAT height )
	{
		HRESULT hr = S_OK;
		WCHAR* PATH_H = L"sWINDOW.hlsl";
		AddBind(std::make_unique<InputLayout>(gfx));
		AddBind(std::make_unique<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));
		AddBind(std::make_unique<VertexShader>(gfx, PATH_H));
		//PATH_H = L"PixelShader.hlsl";
		AddBind(std::make_unique<PixelShader>(gfx, PATH_H));
		AddBind(std::make_unique<Sampler>(gfx));
		// create depth/stencil state
		D3D11_DEPTH_STENCIL_DESC dsdesc = { 0 };
		dsdesc.DepthEnable = false;										//深度测试无所谓
		dsdesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdesc.StencilEnable = true;
		dsdesc.StencilReadMask = 0xff;
		dsdesc.StencilWriteMask = 0xff;
		dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			//Stencil test never pass
		dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//depth fail失败了也+1
		dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			//不会失败，这项无意义。
		//背面被剔除了，下面这些设置无意义。
		dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
		dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		hr = gfx.m_pd3dDevice->CreateDepthStencilState(
			&dsdesc,      // [In]
			&DepthStencilState    // [Out]
		);
		if (FAILED(hr))
			throw "create ppDepthStencilState 1 fail";

		// Setup the viewport
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = topleftx;
		vp.TopLeftY = toplefty;
	};
	~subWin() {};
	void TextureDraw(Graphics& gfx) const noexcept
	{
		for (auto& b : binds)
		{
			b->Bind(gfx);
		}

		gfx.SetCullMode(0);
		gfx.m_pImmediateContext->RSSetViewports(1, &vp);
		gfx.m_pImmediateContext->OMSetDepthStencilState(DepthStencilState, 1);
		
		gfx.SimpleDraw(4);
		gfx.m_pImmediateContext->GSSetShader(nullptr, nullptr, 0u);
		
		//gfx.m_pImmediateContext->OMSetDepthStencilState(NULL, 0);
	}
	void Update(float dt) noexcept 
	{
	}
	XMMATRIX subWin::GetTransformXM() const noexcept
	{
		return XMMatrixIdentity();
	}
private:
	ID3D11DepthStencilState* DepthStencilState = NULL;
	D3D11_VIEWPORT vp;
};


