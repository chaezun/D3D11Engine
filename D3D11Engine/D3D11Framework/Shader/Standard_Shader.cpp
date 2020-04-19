#include "D3D11Framework.h"
#include "Standard_Shader.h"

std::map<Shader_Flags, std::shared_ptr<class Shader>> Standard_Shader::standard_shaders;

auto Standard_Shader::GetMatching_StandardShader(Context * context, const uint & shader_flags) -> std::shared_ptr<class Shader>
{
	auto pair_ib = standard_shaders.insert
	(
		std::make_pair(static_cast<Shader_Flags>(shader_flags), std::make_shared<Shader>(context))
	);

	auto shader = pair_ib.first->second;

	if (pair_ib.second)
	{
		shader->AddDefine("ALBEDO_TEXTURE",		shader_flags & Shader_Flags_Albedo ?	"1" : "0");
		shader->AddDefine("NORMAL_TEXTURE",		shader_flags & Shader_Flags_Normal ?	"1" : "0");

		auto directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Shader);

		shader->AddShader<PixelShader>(directory + "GBuffer.hlsl");
	}

	return shader;
}