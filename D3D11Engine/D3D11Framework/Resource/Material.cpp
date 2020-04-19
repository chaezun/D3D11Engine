#include "D3D11Framework.h"
#include "Material.h"
#include "Shader/Standard_Shader.h"

Material::Material(Context * context)
	: IResource(context, ResourceType::Material)
{
	ZeroMemory(&cpu_buffer, sizeof(MATERIAL_DATA));

	SetStandardShader();
}

Material::~Material()
{
	textures.clear();
}

const bool Material::SaveToFile(const std::string & path)
{
	return false;
}

const bool Material::LoadFromFile(const std::string & path)
{
	return false;
}

void Material::SetStandardShader()
{
	uint shader_flags = 0U;

	if (HasTexture(TextureType::Albedo))    shader_flags |= Shader_Flags_Albedo;
	if (HasTexture(TextureType::Normal))    shader_flags |= Shader_Flags_Normal;

	shader = Standard_Shader::GetMatching_StandardShader(context, shader_flags);
}

auto Material::GetTexture(const TextureType & type) -> std::shared_ptr<ITexture>
{
	return HasTexture(type) ? textures[type] : nullptr;
}

auto Material::GetTextureShaderResource(const TextureType & type) -> ID3D11ShaderResourceView *
{
	auto texture = GetTexture(type);

	return texture ? texture->GetShaderResourceView() : nullptr;
}

auto Material::GetTextureShaderResources() -> std::vector<ID3D11ShaderResourceView *>
{
	std::vector<ID3D11ShaderResourceView*> shader_resources
	{
		GetTextureShaderResource(TextureType::Albedo),
		GetTextureShaderResource(TextureType::Normal),
	};

	return shader_resources;
}

void Material::SetTexture(const TextureType & type, const std::shared_ptr<ITexture>& texture)
{
	if (texture)
		textures[type] = texture;
	else
		textures.erase(type);

	SetStandardShader();
}

void Material::SetTexture(const TextureType & type, const std::string & path)
{
	auto texture = context->GetSubsystem<ResourceManager>()->Load<Texture2D>(path);

	SetTexture(type, texture);
}

auto Material::HasTexture(const TextureType & type) -> const bool
{
	return textures.find(type) != textures.end();
}

auto Material::HasTexture(const std::string & path) -> const bool
{
	for (const auto& texture : textures)
	{
		if (texture.second->GetResourcePath() == path)
			return true;
	}
	return false;
}

void Material::UpdateConstantBuffer()
{
	if (!gpu_buffer)
	{
		gpu_buffer = std::make_shared<ConstantBuffer>(context);
		gpu_buffer->Create<MATERIAL_DATA>();
	}

	auto is_update = false;
	is_update |= cpu_buffer.albedo_color != albedo_color ? true : false;
	is_update |= cpu_buffer.uv_tiling != uv_tiling ? true : false;
	is_update |= cpu_buffer.uv_offset != uv_offset ? true : false;
	is_update |= cpu_buffer.roughness_coef != roughness_coef ? true : false;
	is_update |= cpu_buffer.metallic_coef != metallic_coef ? true : false;
	is_update |= cpu_buffer.normal_coef != normal_coef ? true : false;

	if (!is_update)
		return;

	auto gpu_data = gpu_buffer->Map<MATERIAL_DATA>();
	if (!gpu_data)
	{
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_data->albedo_color = cpu_buffer.albedo_color = albedo_color;
	gpu_data->uv_tiling = cpu_buffer.uv_tiling = uv_tiling;
	gpu_data->uv_offset = cpu_buffer.uv_offset = uv_offset;
	gpu_data->roughness_coef = cpu_buffer.roughness_coef = roughness_coef;
	gpu_data->metallic_coef = cpu_buffer.metallic_coef = metallic_coef;
	gpu_data->normal_coef = cpu_buffer.normal_coef = normal_coef;

	gpu_buffer->Unmap();
}
