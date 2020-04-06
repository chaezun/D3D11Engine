#pragma once

class InputLayout final : public D3D11_Object
{
public:
    InputLayout(class Context* context);
    ~InputLayout();

    auto GetResource() const -> ID3D11InputLayout* { return input_layout; }

    void Create(D3D11_INPUT_ELEMENT_DESC* descs, const uint& count, ID3DBlob* blob);
    void Create(ID3DBlob* blob);
    void Clear();

private:
    ID3D11Device* device            = nullptr;
    ID3D11InputLayout* input_layout = nullptr;
};