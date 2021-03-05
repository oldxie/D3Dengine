#pragma once
#include "Bindable.h"

class ComputeShader : public Bindable
{
public:
	ComputeShader(Graphics& gfx, WCHAR* path , const LPCSTR FunctionName ="CS");
	void Bind(Graphics& gfx, const Resourcedesc* res = nullptr) noexcept override;
	void UnBind(Graphics& gfx) noexcept override;
	//void CopyUAVtoSRV(Graphics& gfx) {};
	//ID3DBlob* GetBytecode() const noexcept;
protected:
	ID3DBlob* pBytecodeBlob;
	ID3D11ComputeShader* pComputeShader;
};