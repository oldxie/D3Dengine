#pragma once
#include "Graphics.h"
#include <vector>

//my def
typedef UINT8 ViewType;
enum Viewtype
{
	ShaderResourceView,
	UnorderedAccessView,
	None,
};
typedef UINT8 LOCATION;
enum location
{
	IB,
	VS,
	HS,
	DS,
	GS,
	OS,
	PS,
	CS,
};
typedef struct Resourcedesc
{
	UINT16 passnum;
	location loc;
	ViewType vt;
}ResourceDesc;

class Bindable
{
public:
	virtual void Bind(Graphics& gfx,const Resourcedesc* res =nullptr) noexcept = 0;
	virtual ~Bindable() = default;
	virtual void CopyUAVtoSRV(Graphics& gfx) {};
	virtual void getResource(Graphics& gfx, ID3D11Buffer* buffer) {};
	void* son = NULL;
	virtual void UnBind(Graphics& gfx) noexcept = 0;
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	//std::map<LOCATION, ViewType> bindlocation;
}; 

//class ComputeBindable : public Bindable
//{
//public:
//	virtual void Bind(Graphics& gfx) noexcept = 0;
//	virtual ~ComputeBindable() = default;
//	virtual void CopyUAVtoSRV(Graphics& gfx) = 0 ;
////protected:
////	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
////	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
////	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
//
//};






