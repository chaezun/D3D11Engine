#include "Framework.h"
#include "IShader.h"

IShader::IShader(Context * context)
{
    device = context->GetSubsystem<Graphics>()->GetDevice();
}

auto IShader::ShaderErrorHandler(const HRESULT & hr, ID3DBlob * error) -> const bool
{
    if (FAILED(hr))
    {
        if (error)
        {
            std::string str = reinterpret_cast<const char*>(error->GetBufferPointer());
            MessageBoxA(nullptr, str.c_str(), "Shader Error!!", MB_OK);
        }
        return false;
    }
    return true;
}

auto IShader::CompileShader(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines, ID3DBlob ** blob) -> const bool
{
#ifdef _DEBUG
    uint flags = D3DCOMPILE_ENABLE_STRICTNESS;
#else
    uint flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ID3DBlob* error = nullptr;
    auto hr = D3DCompileFromFile
    (
        FileSystem::ToWString(path).c_str(),
        defines,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        function_name.c_str(),
        profile.c_str(),
        flags,
        0,
        blob,
        &error
    );

    auto result = ShaderErrorHandler(hr, error);

    SAFE_RELEASE(error);

    return result;
}
