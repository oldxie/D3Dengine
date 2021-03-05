#include "Drawable.h"
//#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::DrawtoSub(Graphics& gfx , UINT vp_index) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexedToVP(pIndexBuffer->GetCount(), vp_index);
}

void Drawable::SimpleDraw(Graphics& gfx, UINT num) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.SimpleDraw(num);
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

UINT16 Drawable::AddResourceAndDesc(std::unique_ptr<Bindable> bind, ResourceDesc* ResDesc, UINT16 ResDescSize) noexcept
{
	if (typeid(*bind) != typeid(IndexBuffer))
	{
		binds.push_back(std::move(bind));
	}
	else
	{
		pIndexBuffer = dynamic_cast<IndexBuffer *> (bind.get());
		binds.push_back(std::move(bind));
	}
	//PassBindList[passnum].push_back
	UINT16 resid= binds.size() - 1;
	if (ResDescSize > 0)
	{
		for (UINT16 i = 0; i < ResDescSize; i++)
		{
			BindDesc binddesc;
			binddesc.needlocandvt = true;
			binddesc.resindex = resid;
			binddesc.res.loc = ResDesc[i].loc;
			binddesc.res.vt = ResDesc[i].vt;

			if (binddescs.find(ResDesc[i].passnum) == binddescs.end())
			{
				binddescs.insert({ ResDesc[i].passnum ,{ binddesc } });
			}
			else
			{
				binddescs[ResDesc[i].passnum].push_back(binddesc);
			}
		}
	}	
	return resid;
}
UINT16 Drawable::AddResource(std::unique_ptr<Bindable> bind, UINT16 passnum) noexcept
{
	if (typeid(*bind) != typeid(IndexBuffer))
	{
		binds.push_back(std::move(bind));
	}
	else
	{
		pIndexBuffer = dynamic_cast<IndexBuffer *> (bind.get());
		binds.push_back(std::move(bind));
	}
	
			BindDesc binddesc;
			binddesc.needlocandvt = false;
			UINT16 resid = binds.size() - 1;
			binddesc.resindex = resid;

			if (binddescs.find(passnum) == binddescs.end())
			{
				binddescs.insert({ passnum ,{ binddesc } });
			}
			else
			{
				binddescs[passnum].push_back(binddesc);
			}
	return resid;
}
void Drawable::AddResourceByID(UINT16 resid, ResourceDesc* Desc)noexcept
{
	BindDesc binddesc;
	binddesc.needlocandvt = false;
	binddesc.resindex = resid;
	binddescs[Desc->passnum].push_back(binddesc);
}

XMMATRIX Drawable::GetCameraPos()  const noexcept
{
	XMVECTOR Eye = XMVectorSet(100.0f, 100.0f, 100.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX re = XMMatrixLookAtLH(Eye, At, Up);
	return re;
}
