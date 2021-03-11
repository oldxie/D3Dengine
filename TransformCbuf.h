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
	struct Cameramatrix
	{
		XMMATRIX MVP;
		XMMATRIX M;
		XMMATRIX V;
		XMMATRIX P;
		XMVECTOR POS;
	};
	Cameramatrix trans;
	VertexConstantBuffer<Cameramatrix> vcbuf;
	PixelConstantBuffer<Cameramatrix> pcbuf;
	const Drawable& parent;
};