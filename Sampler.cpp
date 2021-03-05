#include "Sampler.h"


void Sampler::Bind(Graphics& gfx, const Resourcedesc* res) noexcept
{
	if (res)
	{
		switch (res->loc)
		{
		case VS:
			GetContext(gfx)->VSSetSamplers(0, 1, &pSamplerLinear);
			break;
		case PS:
			GetContext(gfx)->PSSetSamplers(0, 1, &pSamplerLinear);
			break;
		default:

			break;
		}
	}
	else GetContext(gfx)->PSSetSamplers(0, 1, &pSamplerLinear);
	
}

