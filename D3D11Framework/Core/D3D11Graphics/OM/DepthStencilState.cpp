#include "D3D11Framework.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(Context * context)
{
	device = context->GetSubsystem<Graphics>()->GetDevice();
}

DepthStencilState::~DepthStencilState()
{
	Clear();
}

void DepthStencilState::Create(const bool & is_depth_enable, const D3D11_COMPARISON_FUNC & comparison_func)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable = is_depth_enable;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = comparison_func;
	desc.StencilEnable = false;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace = desc.FrontFace;

	auto result = SUCCEEDED(device->CreateDepthStencilState(&desc, &state));
	if (!result)
		LOG_ERROR("Failed to create depth_stencil state");
}

void DepthStencilState::Clear()
{
	SAFE_RELEASE(state);
}
