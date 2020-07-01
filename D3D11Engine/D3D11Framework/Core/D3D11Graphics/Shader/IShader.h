#pragma once

class IShader
{
public:
    IShader(class Context* context);
    virtual ~IShader() = default;

    virtual void Create
    (
        const std::string& path,
        const std::string& function_name,
        const std::string& profile,
        D3D_SHADER_MACRO* defines
    ) = 0;
    virtual void Clear() = 0;
    
protected:
    virtual auto ShaderErrorHandler(const HRESULT& hr, ID3DBlob* error) -> const bool;
    virtual auto CompileShader
    (
        const std::string& path,
        const std::string& function_name,
        const std::string& profile,
        D3D_SHADER_MACRO* defines,
        ID3DBlob** blob
    ) -> const bool;

protected:
    ID3D11Device* device        = nullptr;
    std::string path            = NOT_ASSIGNED_STR;
    std::string function_name   = NOT_ASSIGNED_STR;
    std::string profile         = NOT_ASSIGNED_STR;
};