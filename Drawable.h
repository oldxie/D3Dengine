#pragma once
#include "Graphics.h"
#include "Bindable.h"
#include <unordered_map>
class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	void DrawtoSub(Graphics& gfx ,UINT vp_index) const noexcept;
	void SimpleDraw(Graphics& gfx, UINT num) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	
	virtual ~Drawable() = default;
	XMMATRIX GetCameraPos() const noexcept;
protected:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
	
	typedef struct Binddesc
	{
		UINT16 resindex;
		bool needlocandvt;
		Resourcedesc res;
	}BindDesc;
	
	std::unordered_map<UINT16 ,std::vector<Binddesc>> binddescs;
	UINT16 AddResourceAndDesc(std::unique_ptr<Bindable> bind, ResourceDesc* Desc ,UINT16 DescSize = 0) noexcept;
	UINT16 AddResource(std::unique_ptr<Bindable> bind, UINT16 passnum) noexcept;
	void AddResourceByID(UINT16 resid, ResourceDesc* Desc)noexcept;
};


