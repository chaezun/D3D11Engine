#include "Framework.h"
#include "Shader.h"

Shader::Shader(Context * context)
   :context(context)
{
}

Shader::~Shader()
{
   shaders.clear();
   defines.clear();
}

void Shader::AddDefine(const std::string & define, const std::string & value)
{
   defines[define] = value;
}

template<typename T>
constexpr ShaderScope Shader::DeduceShaderScope()
{
	return ShaderScope::Unknown;
}

#define REGISTER_SHADER_TYPE(T, enum_type) template <> ShaderScope Shader::DeduceShaderScope<T>() {return enum_type;}

REGISTER_SHADER_TYPE(VertexShader, ShaderScope::VS)
REGISTER_SHADER_TYPE(PixelShader, ShaderScope::PS)