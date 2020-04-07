#include "D3D11Framework.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(Context * context)
{
    device          = context->GetSubsystem<Graphics>()->GetDevice();
    device_context  = context->GetSubsystem<Graphics>()->GetDeviceContext();
}

ConstantBuffer::~ConstantBuffer()
{
    Clear();
}

void ConstantBuffer::Clear()
{
    SAFE_RELEASE(buffer);
}

void ConstantBuffer::Unmap()
{
    device_context->Unmap(buffer, 0);
}
