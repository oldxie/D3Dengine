#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <xnamath.h>

class TransformCbuf : public Bindable
{
public:
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override {}
private:
	VertexConstantBuffer<XMMATRIX> vcbuf;
	const Drawable& parent;
};