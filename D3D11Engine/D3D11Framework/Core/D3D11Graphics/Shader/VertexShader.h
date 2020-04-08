#pragma once
#include "IShader.h"

class VertexShader final : public IShader, public D3D11_Object
{
public:
    VertexShader(class Context* context);
    ~VertexShader();

    auto GetResource() const -> ID3D11VertexShader* { return shader; }
    auto GetBlob() const -> ID3DBlob* { return blob; }

    void Create(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines) override;
    void Clear() override;

private:
    ID3D11VertexShader* shader  = nullptr;
    ID3DBlob* blob              = nullptr;
};