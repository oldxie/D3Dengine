#pragma once
#include "Bindable.h"

class HullShader : public Bindable
{
public:
	HullShader(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept;
protected:
	ID3D11HullShader* pHullShader;
private:
	ID3DBlob* pBytecodeBlob;
};

class DomainShader : public Bindable
{
public:
	DomainShader(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx , const Resourcedesc* res = nullptr ) noexcept override;
	void UnBind(Graphics& gfx) noexcept;
protected:
	ID3D11DomainShader* pDomainShader;
private:
	ID3DBlob* pBytecodeBlob;
};