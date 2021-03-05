#include "Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type(type)
{}

void Topology::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(type);
}
void Topology::UnBind(Graphics& gfx) noexcept
{
	//GetContext(gfx)->IASetPrimitiveTopology(0);
}
