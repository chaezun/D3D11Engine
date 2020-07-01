#pragma once
#include "IResource.h"

enum class TextureType : uint
{
	Unknown,
	//입체감이 없이 색만 가지고 있는 것
	Albedo,
	Normal,
	Height,
};

class Material final : public IResource
{
public:
	Material(class Context* context);
	~Material();

	const bool SaveToFile(const std::string& path) override;
	const bool LoadFromFile(const std::string& path) override;

	//======================================================================================
	// [Shader]
	//======================================================================================
	auto GetShader() -> const std::shared_ptr<class Shader>& { return shader; }
	void SetShader(const std::shared_ptr<class Shader>& shader) { this->shader = shader; }
	void SetStandardShader();

	//======================================================================================
	// [Texture]
	//======================================================================================
	auto GetTexture(const TextureType& type)->std::shared_ptr<ITexture>;
	auto GetTextureShaderResource(const TextureType& type)->ID3D11ShaderResourceView*;
	auto GetTextureShaderResources()->std::vector<ID3D11ShaderResourceView*>;

	void SetTexture(const TextureType& type, const std::shared_ptr<ITexture>& texture);
	void SetTexture(const TextureType& type, const std::string& path);

	auto HasTexture(const TextureType& type) -> const bool;
	auto HasTexture(const std::string& path) -> const bool;

	//======================================================================================
	// [Property]
	//======================================================================================
	auto GetCullMode() const -> const D3D11_CULL_MODE& { return cull_mode; }
	void SetCullMode(const D3D11_CULL_MODE& cull_mode) { this->cull_mode = cull_mode; }

	auto GetRoughnessCoefficient() const -> const float& { return roughness_coef; }
	void SetRoughnessCoefficient(const float& roughness_coef) { this->roughness_coef = roughness_coef; }

	auto GetMetallicCoefficient() const -> const float& { return metallic_coef; }
	void SetMetallicCoefficient(const float& metallic_coef) { this->metallic_coef = metallic_coef; }

	auto GetNormalCoefficient() const -> const float& { return normal_coef; }
	void SetNormalCoefficient(const float& normal_coef) { this->normal_coef = normal_coef; }

	auto GetHeightCoefficient() const ->const float& { return height_coef; }
	void SetHeightCoefficient(const float& height_coef) { this->height_coef = height_coef; }

	auto GetAlbedoColor() const ->const Color4& { return albedo_color; }
	void SetAlbedoColor(const Color4& albedo_color) { this->albedo_color = albedo_color; }

	auto GetTiling() const ->const Vector2& { return uv_tiling; }
	void SetTiling(const Vector2& uv_tiling) { this->uv_tiling = uv_tiling; }

	auto GetOffset() const ->const Vector2& { return uv_offset; }
	void SetOffset(const Vector2& uv_offset) { this->uv_offset = uv_offset; }

	//======================================================================================
	// [Constant Buffer]
	//======================================================================================
	auto GetConstantBuffer() const -> const std::shared_ptr<ConstantBuffer>& { return gpu_buffer; }
	void UpdateConstantBuffer();

private:
	std::shared_ptr<class Shader> shader;

	D3D11_CULL_MODE cull_mode = D3D11_CULL_BACK;

	Color4 albedo_color = 1.0f;
	float roughness_coef = 1.0f; // coefficient : 계수
	float metallic_coef = 0.0f;
	float normal_coef = 0.0f;
	float height_coef = 0.0f;
	Vector2 uv_tiling = 1.0f;
	Vector2 uv_offset = 0.0f;

	std::map<TextureType, std::shared_ptr<ITexture>> textures;

	//======================================================================================
	// [Constant Buffer]
	//======================================================================================
	MATERIAL_DATA cpu_buffer;
	std::shared_ptr<ConstantBuffer> gpu_buffer;
};