#pragma once
#include "IComponent.h"

enum class SkyBoxType : uint
{
	Cubemap,
	Sphere,
};

class SkyBox final : public IComponent
{
public:
	SkyBox
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~SkyBox() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetTexture() -> const std::shared_ptr<class ITexture>& { return texture; }
	void SetTexture(const std::shared_ptr<class ITexture>& texture);

	void SetStandardSkyBox();

	//
	auto GetMaterial() const -> const std::shared_ptr<class Material>& { return material; }

private:
	void CreateFromCubemap(const std::vector<std::string>& paths);
	void CreateFromSphere(const std::string& path);

private:
	SkyBoxType sky_box_type = SkyBoxType::Sphere;
	std::vector<std::string> texture_paths;
	std::shared_ptr<class ITexture> texture;

	//
	std::shared_ptr<class Material> material;
};