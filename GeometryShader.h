#pragma once
#include "Bindable.h"

class GeometryShader : public Bindable
{
public:
	GeometryShader(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept;
	ID3DBlob* GetBytecode() const noexcept;
protected:
	ID3DBlob* pBytecodeBlob;
	ID3D11GeometryShader* pGeometryShader;
};


class GeometryShaderWithSO : public Bindable
{
public:
	GeometryShaderWithSO(Graphics& gfx, WCHAR* path);
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr ) noexcept override;
	void SetParameter(Graphics& gfx, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, UINT pBufferStrides);
	void UnBind(Graphics& gfx) noexcept;
protected:
	ID3DBlob* pBytecodeBlob;
	ID3D11GeometryShader* pGeometryShader;
};