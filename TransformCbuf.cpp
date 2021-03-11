#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	pcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	trans.MVP = XMMatrixTranspose (gfx.GetCamera()->GetTransMatrix()* parent.GetTransformXM() * gfx.GetProjection());
	trans.V = XMMatrixTranspose (gfx.GetCamera()->GetTransMatrix());
	trans.M = XMMatrixTranspose(parent.GetTransformXM());
	trans.P = XMMatrixTranspose(gfx.GetProjection());
	trans.POS = gfx.GetCamera()->GetCameraPos();
	vcbuf.Update(gfx,trans);
	vcbuf.Bind(gfx);
	pcbuf.Update(gfx, trans);
	pcbuf.Bind(gfx);
}
