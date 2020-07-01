#include "D3D11Framework.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Context * context)
{
    device = context->GetSubsystem<Graphics>()->GetDevice();
}

IndexBuffer::~IndexBuffer()
{
    Clear();
}

void IndexBuffer::Create(const std::vector<uint>& indices, const D3D11_USAGE & usage)
{
	stride = sizeof(uint);
	count = static_cast<uint>(indices.size());

	//버퍼를 정의(index buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //버퍼의 접근에 관련
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER; //=index buffer로 사용
	desc.ByteWidth = stride * count; //크기

	//버퍼의 접근에 관련된 flag
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT:
	case D3D11_USAGE_IMMUTABLE: desc.CPUAccessFlags = 0;                                                break;
	case D3D11_USAGE_DYNAMIC:   desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                           break;
	case D3D11_USAGE_STAGING:   desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;   break;
	}

	//리소스 내에 있는 실제 데이터를 정의
	D3D11_SUBRESOURCE_DATA sub_data;
	ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
	sub_data.pSysMem = indices.data(); //버퍼 데이터 초기화

	auto hr = device->CreateBuffer(&desc, &sub_data, &buffer);
	assert(SUCCEEDED(hr));
}

void IndexBuffer::Clear()
{
    SAFE_RELEASE(buffer);
    stride  = 0;
    offset  = 0;
    count   = 0;
}
