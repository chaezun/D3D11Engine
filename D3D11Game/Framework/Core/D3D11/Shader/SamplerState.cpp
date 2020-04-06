#include "Framework.h"
#include "SamplerState.h"

SamplerState::SamplerState(Context * context)
{
	device = context->GetSubsystem<Graphics>()->GetDevice();
}

SamplerState::~SamplerState()
{
	Clear();
}

void SamplerState::Create(const D3D11_FILTER & filter, const D3D11_TEXTURE_ADDRESS_MODE & address_mode, const D3D11_COMPARISON_FUNC & comparison_func)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter = filter;
	desc.AddressU = address_mode;
	desc.AddressV = address_mode;
	desc.AddressW = address_mode;
	desc.ComparisonFunc=comparison_func;
	desc.MaxAnisotropy = 16;
	//BorderColor: UV좌표를 벗어나는 영역을 칠할 색을 정함
	desc.BorderColor[0]=0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;
	desc.MinLOD = std::numeric_limits<float>::min();
	desc.MaxLOD =std::numeric_limits<float>::max();
	//LOD : Level of Detail
	desc.MipLODBias=0.0f;

	auto result = SUCCEEDED(device->CreateSamplerState(&desc, &state));
	if(!result)
	  LOG_ERROR("Failed to create sampler state");
}

void SamplerState::Clear()
{
	SAFE_RELEASE(state);
}
