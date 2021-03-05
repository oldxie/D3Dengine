#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics& gfx, const Resourcedesc* res ) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, &pVertexBuffer , &stride, &offset);
}
