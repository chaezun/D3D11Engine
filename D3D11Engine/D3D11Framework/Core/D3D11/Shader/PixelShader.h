#pragma once
#include "IShader.h"

class PixelShader final : public IShader, public D3D11_Object
{
public:
    PixelShader(class Context* context);
    ~PixelShader();

    auto GetResource() const -> ID3D11PixelShader* { return shader; }
    auto GetBlob() const -> ID3DBlob* { return blob; }

    void Create(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines) override;
    void Clear() override;

private:
    ID3D11PixelShader* shader   = nullptr;
    ID3DBlob* blob              = nullptr;
};