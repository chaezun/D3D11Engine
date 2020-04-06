#pragma once

class ConstantBuffer final : public D3D11_Object
{
public:
    ConstantBuffer(class Context* context);
    ~ConstantBuffer();

    auto GetResource() const -> ID3D11Buffer* { return buffer; }

    template <typename T>
    void Create(const D3D11_USAGE& usage = D3D11_USAGE_DYNAMIC);
    void Clear();

    template <typename T>
    auto Map() -> T*;
    void Unmap();

private:
    ID3D11Device* device                    = nullptr;
    ID3D11DeviceContext* device_context     = nullptr;
    ID3D11Buffer* buffer                    = nullptr;
};

template<typename T>
inline void ConstantBuffer::Create(const D3D11_USAGE & usage)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage      = usage;
    desc.BindFlags  = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth  = sizeof(T);

    switch (usage)
    {
    case D3D11_USAGE_DEFAULT:
    case D3D11_USAGE_IMMUTABLE: desc.CPUAccessFlags = 0;                                                break;
    case D3D11_USAGE_DYNAMIC:   desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                           break;
    case D3D11_USAGE_STAGING:   desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;   break;
    }

    auto hr = device->CreateBuffer(&desc, nullptr, &buffer);
    assert(SUCCEEDED(hr));
}

template<typename T>
inline auto ConstantBuffer::Map() -> T *
{
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    auto hr = device_context->Map
    (
        buffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped_resource
    );
    assert(SUCCEEDED(hr));

    return reinterpret_cast<T*>(mapped_resource.pData);
}
