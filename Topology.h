#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type;
};