#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;
protected:
	ID3DBlob* pBytecodeBlob;
	ID3D11VertexShader* pVertexShader;
};