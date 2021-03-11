#pragma once
#include "Graphics.h"
#include "Bindable.h"
#include <unordered_map>
class Bindable;

#define ResouceID UINT16


class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual XMMATRIX GetTransformXM() const noexcept = 0;
	//Graphics& gfxin;
	void Submit(Graphics& gfx) const noexcept;
	
	virtual void Update(float dt, Graphics& gfx) noexcept = 0;

	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	
	virtual ~Drawable() = default;
	XMMATRIX GetCameraPos() const noexcept;
protected:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
	
	enum DrawClass
	{
		dispatch,
		draw,
		drawindex,
		drawauto
	};
	typedef struct Binddesc
	{
		UINT16 resindex;
		bool needlocandvt;
		Resourcedesc res;
	}BindDesc;

	typedef struct Drawdesc
	{
		UINT16 drawtype;//0: dispatch ; 1 : draw;
		union  
		{
			struct {
				UINT16 dispatch_x;
				UINT16 dispatch_y;
				UINT16 dispatch_z;
			};
			struct {
				UINT32 vertexsize;
				UINT16 reserve;
			};
		} drawsize;
		const D3D11_VIEWPORT * vp;
		UINT8 cullmode;
	}DrawDesc;
	
	struct Passdesc
	{
		std::vector<Binddesc> binddescs;
		DrawDesc drawdescs;
	};
	
	std::unordered_map<UINT16, Passdesc> passdescs;

	class ComputePipe
	{
		
	};

	std::unordered_map<LPCSTR, std::unique_ptr<Bindable>> resoucemap;
	//Create Resource with name
	void CreateResource(LPCSTR resourcename ,std::unique_ptr<Bindable> bind)  noexcept;
	//Set/bind Resorce (by description)
	void SetResource(Graphics& gfx , LPCSTR resourcename, location loc = Nolocation, ViewType vt = None, UINT slot = 0)  noexcept;

	
	void* globalpara;

	void BindtoUpdate(ResouceID resid) noexcept;
	ResouceID updateresource;

	//dispath and draw, not actually draw, only descript the draw
	void Dispatch(Graphics& gfx ,UINT elements_x, UINT elements_y, UINT elements_z) noexcept;
	void Draw(Graphics& gfx,DrawClass drawtype, const D3D11_VIEWPORT *vp, UINT32 vertexsize, UINT8 cullmode)noexcept;
	
private:
	
	UINT16 passnum=0;
	std::vector<LPCSTR> binddesclist;
};


