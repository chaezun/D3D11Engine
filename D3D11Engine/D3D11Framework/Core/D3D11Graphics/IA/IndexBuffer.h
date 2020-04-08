#pragma once

class IndexBuffer final : public D3D11_Object
{
public:
    IndexBuffer(class Context* context);
    ~IndexBuffer();

    auto GetResource() const -> ID3D11Buffer* { return buffer; }
    auto GetStride() const -> const uint& { return stride; }
    auto GetOffset() const -> const uint& { return offset; }
    auto GetCount() const -> const uint& { return count; }

    void Create(const std::vector<uint>& indices, const D3D11_USAGE& usage = D3D11_USAGE_DEFAULT);
    void Clear();

private:
    ID3D11Device* device    = nullptr;
    ID3D11Buffer* buffer    = nullptr;
    uint stride             = 0;
    uint offset             = 0;
    uint count              = 0;
};