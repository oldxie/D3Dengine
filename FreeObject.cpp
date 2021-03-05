#include "FreeObject.h"
//#include "Bindable.h"

FreeObject::FreeObject(Graphics& gfx)
	:
	device(gfx)
{
	
}
FreeObject::~FreeObject()
{
}

void FreeObject::Update(float dt) noexcept
{
	
}

XMMATRIX FreeObject::GetTransformXM() const noexcept
{
	return XMMatrixIdentity();
}