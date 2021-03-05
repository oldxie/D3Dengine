#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	vcbuf.Update(gfx,
		XMMatrixTranspose(
			gfx.GetCamera()->GetTransMatrix()* parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	vcbuf.Bind(gfx);
}
