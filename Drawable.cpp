#include "Drawable.h"
//#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
#include <string.h>

void Drawable::Submit(Graphics& gfx) const noexcept
{
	for (UINT16 passnum = 0; passnum < passdescs.size(); passnum++)
	{
		for (auto& desc : passdescs.at(passnum).binddescs)
		{
			binds.at(desc.resindex)->Bind(gfx, &desc.res);
		}
		//DrawDesc drawdesc = passdescs.at(passnum).drawdescs;
		if (passdescs.at(passnum).drawdescs.vp != NULL)
		{	
			gfx.m_pImmediateContext->RSSetViewports(1, passdescs.at(passnum).drawdescs.vp);
			gfx.SetCullMode(passdescs.at(passnum).drawdescs.cullmode);
		}
		switch (passdescs.at(passnum).drawdescs.drawtype)
		{
		case dispatch :
			gfx.Dispatch(passdescs.at(passnum).drawdescs.drawsize.dispatch_x, passdescs.at(passnum).drawdescs.drawsize.dispatch_y, passdescs.at(passnum).drawdescs.drawsize.dispatch_z);
			break;
		case draw : 
			gfx.SimpleDraw(passdescs.at(passnum).drawdescs.drawsize.vertexsize);
			break;
		case drawindex:
		case drawauto:
		default:
			break;
		}

		for (auto& desc : passdescs.at(passnum).binddescs)
		{
			binds.at(desc.resindex)->UnBind(gfx);
		}
	}
	
}

void Drawable::BindtoUpdate(ResouceID resid) noexcept
{
	updateresource=resid;
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}

void Drawable::CreateResource(LPCSTR resourcename, std::unique_ptr<Bindable> bind)  noexcept
{
	if (bind == nullptr)
	{
		resoucemap[resourcename] = nullptr;
	}
	else if (typeid(*bind) != typeid(IndexBuffer))
	{
		resoucemap[resourcename] = std::move(bind);
	}
	else
	{
		pIndexBuffer = dynamic_cast<IndexBuffer *> (bind.get());
		resoucemap[resourcename]= std::move(bind);
	}
}
void Drawable::SetResource(Graphics& gfx ,LPCSTR resourcename, location loc, ViewType vt, UINT slot)  noexcept
{
	ResourceDesc res;
	res.slot = slot;
	res.loc = loc;
	res.vt = vt;
	resoucemap[resourcename]->Bind(gfx, &res);
	binddesclist.push_back(resourcename);
}

void Drawable::Dispatch(Graphics& gfx,UINT elements_x, UINT elements_y, UINT elements_z) noexcept
{
	gfx.Dispatch(elements_x, elements_y, elements_z);
	for (auto* resname : binddesclist)
	{
		resoucemap[resname]->UnBind(gfx);
	}
	binddesclist.clear();

}

void Drawable::Draw(Graphics& gfx, DrawClass drawtype , const D3D11_VIEWPORT *vp ,UINT32 vertexsize, UINT8 cullmode) noexcept
{
	gfx.m_pImmediateContext->RSSetViewports(1, vp);
	gfx.SetCullMode(cullmode);
	switch (drawtype)
	{
	case draw:
		gfx.SimpleDraw(vertexsize);
		break;
	case drawindex:
	case drawauto:
	default:
		break;
	}
	for (auto* resname : binddesclist)
	{
		resoucemap[resname]->UnBind(gfx);
	}
	binddesclist.clear();

}
XMMATRIX Drawable::GetCameraPos()  const noexcept
{
	XMVECTOR Eye = XMVectorSet(100.0f, 100.0f, 100.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX re = XMMatrixLookAtLH(Eye, At, Up);
	return re;
}
