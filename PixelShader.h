#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override;
protected:
	ID3D11PixelShader* pPixelShader;
};