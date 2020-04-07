#include "D3D11Framework.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState(Context * context)
{
   device=context->GetSubsystem<Graphics>()->GetDevice();
}

RasterizerState::~RasterizerState()
{
   Clear();
}

void RasterizerState::Create(const D3D11_CULL_MODE & cull_mode, const D3D11_FILL_MODE & fill_mode)
{
   D3D11_RASTERIZER_DESC desc;
   ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
   desc.CullMode		      = cull_mode;
   desc.FillMode		      = fill_mode;
   desc.DepthClipEnable       = true;
   desc.FrontCounterClockwise = false;

   auto result = SUCCEEDED(device->CreateRasterizerState(&desc, &state));
   if(!result)
      LOG_ERROR("Failed to create rasterizer state");
}

void RasterizerState::Clear()
{
   SAFE_RELEASE(state);
}
