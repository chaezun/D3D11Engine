#include "Framework.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Context * context)
{
    device = context->GetSubsystem<Graphics>()->GetDevice();
	device_context = context->GetSubsystem<Graphics>()->GetDeviceContext();
}

VertexBuffer::~VertexBuffer()
{
    Clear();
}

auto VertexBuffer::Map() -> void *
{
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	auto result = SUCCEEDED(device_context->Map
	(
		buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_subresource
	));

	if(!result)
	  LOG_ERROR("Failed to map buffer");

	return mapped_subresource.pData;
}

void VertexBuffer::Unmap()
{
	device_context->Unmap(buffer, 0);

}

void VertexBuffer::Clear()
{
    SAFE_RELEASE(buffer);
    stride  = 0;
    offset  = 0;
    count   = 0;
}
