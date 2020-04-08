#include "D3D11Framework.h"
#include "VertexShader.h"

VertexShader::VertexShader(Context * context)
    : IShader(context)
{
}

VertexShader::~VertexShader()
{
    Clear();
}

void VertexShader::Create(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines = nullptr)
{
    this->path          = path;
    this->function_name = function_name != NOT_ASSIGNED_STR ? function_name : "VS";
    this->profile       = profile       != NOT_ASSIGNED_STR ? profile       : "vs_5_0";

	//IShader의 가상함수 CompileShader 호출
    auto result = CompileShader(this->path, this->function_name, this->profile, defines, &blob);
    
	//Vertex Shader 컴파일에 실패했을 경우
	if (!result)
        return;

	//Vertex Shader 컴파일에 성공했을 경우
    auto hr = device->CreateVertexShader
    (
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &shader
    );
    assert(SUCCEEDED(hr));
}

void VertexShader::Clear()
{
    SAFE_RELEASE(shader);
    SAFE_RELEASE(blob);
    path            = NOT_ASSIGNED_STR;
    function_name   = NOT_ASSIGNED_STR;
    profile         = NOT_ASSIGNED_STR;
}
