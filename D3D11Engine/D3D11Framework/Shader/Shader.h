#pragma once
#include "Core/D3D11Graphics/Shader/IShader.h"

//여러 shader를 관리하는 클래스
class Shader final : public D3D11_Object
{
public:
    template<typename T>
	static constexpr ShaderScope DeduceShaderScope();

public: 
	Shader(class Context* context);
	~Shader();

	auto GetInputLayout() -> std::shared_ptr<InputLayout> {return input_layout;}

	template <typename T>
	auto GetShader() -> std::shared_ptr<T>;

	template <typename T>
	void AddShader
	(
	  const std::string path,
	  const std::string& function_name=NOT_ASSIGNED_STR,
	  const std::string& profile      =NOT_ASSIGNED_STR
	);

	void AddDefine
	(
	   const std::string& define,
	   const std::string& value="1"
	);

private:
    class Context* context;

	std::shared_ptr<InputLayout> input_layout;
	std::map<ShaderScope, std::shared_ptr<IShader>> shaders;
	std::map<std::string, std::string> defines;
};

template<typename T>
inline auto Shader::GetShader() -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IShader,T>::value, "Provided type does not implement IShader");

	auto shader_scope=DeduceShaderScope<T>();

	if (shaders.find(shader_scope) == shaders.end())
		return nullptr;
	

	return std::static_pointer_cast<T>(shaders[shader_scope]);
}

template<typename T>
inline void Shader::AddShader(const std::string path, const std::string & function_name, const std::string & profile)
{
	static_assert(std::is_base_of<IShader, T>::value, "Provided type does not implement IShader");

    //Create Shader Macro
    std::vector<D3D_SHADER_MACRO> shader_macros;

	for(auto& define : defines)
	{ 
	   shader_macros.emplace_back(D3D_SHADER_MACRO{define.first.c_str(),define.second.c_str()});
	}
    
	shader_macros.emplace_back(D3D_SHADER_MACRO{nullptr,nullptr});

	//Create Shader
	auto shader_scope= DeduceShaderScope<T>();
	//i: iterator, b: bool
	auto pair_ib=shaders.insert(std::make_pair(shader_scope,std::make_shared<T>(context)));
	auto shader=std::static_pointer_cast<T>(pair_ib.first->second);

	if(pair_ib.second)
	     shader->Create(path, function_name, profile, shader_macros.data());

	//중복되는 정보가 들어올 경우
	else
	{
	   shader->Clear();
	   shader->Create(path, function_name, profile, shader_macros.data());
	}

	if constexpr (std::is_same<T, VertexShader>::value)
	{
	    if(!input_layout)
		   input_layout=std::make_shared<InputLayout>(context);

		input_layout->Clear();
		input_layout->Create(shader->GetBlob());
	}
}
