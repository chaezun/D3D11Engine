#pragma once
#include "ISubsystem.h"
#include "Resource/IResource.h"

enum class AssetType :uint
{
	Shader,
	Texture,
	Icon,
	Model,
	Audio,
	Cubemap,
	Font,
	Script,
};

//��� asset�� �����ϴ� Ŭ����
class ResourceManager final : public ISubsystem
{
public:
	ResourceManager(class Context* context);
	~ResourceManager() = default;

	const bool Initialize() override;

	//============================================================================================
	// [Importer]
	//============================================================================================
	auto GetTextureImporter() -> const  std::shared_ptr<class TextureImporter>& { return texture_importer; }
	auto GetModelImporter() -> const  std::shared_ptr<class ModelImporter>& { return model_importer; }
    
	//============================================================================================
	// [Resource]
	//============================================================================================
	template <typename T>
	auto Load(const std::string& path)->std::shared_ptr<T>;

	template <typename T>
	auto GetResourceFromName(const std::string& name)->std::shared_ptr<T>;

	template <typename T>
	auto GetResourceFromPath(const std::string& path)->std::shared_ptr<T>;

	template <typename T>
	void RegisterResource(std::shared_ptr<T>& resource);

	auto HasResource(const std::string& resource_name, const ResourceType& resource_type)->const bool;
	//============================================================================================
	// [Directory]
	//============================================================================================
	auto GetProjectDirectory() const -> const std::string& { return project_directory; }
	void SetProjectDirectory(const std::string& directory);

	auto GetAssetDirectory() -> const std::string { return std::string("../../../_Assets/"); }
	auto GetAssetDirectory(const AssetType& type) -> const std::string;

	void RegisterDirectory(const AssetType& type, const std::string& directory);

private:
	typedef std::vector<std::shared_ptr<IResource>> resource_group;
	std::map<ResourceType, resource_group> resource_groups;
	std::mutex resource_mutex;

	std::map<AssetType, std::string> asset_directories;
	std::string project_directory;

	std::shared_ptr<class TextureImporter> texture_importer;
	std::shared_ptr<class ModelImporter> model_importer;

};

template<typename T>
inline auto ResourceManager::Load(const std::string & path) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource"); //�����Ϸ��� Ŭ������ IResource�� �Ļ�Ŭ������ �ƴ϶�� ��

	if (!FileSystem::IsExistFile(path)) //������ �����ϴ��� �Ǻ�
	{
		return nullptr;
	}

	auto relative_path = FileSystem::GetRelativeFromPath(path); //��θ� ����η� ��ȯ
	auto resource_name = FileSystem::GetIntactFileNameFromPath(relative_path); //����θ� ���� �����̸��� ����

	if (HasResource(resource_name, IResource::DeduceResourceType<T>())) //���ҽ� �̸��� ���ҽ� Ÿ���� ��ġ�ϴ� ���� �ִ��� �Ǻ�
		return GetResourceFromName<T>(resource_name);

	else //��ġ�ϴ� ���� ���ٸ� ���� ������
	{
		auto resource = std::make_shared<T>(context);
		resource->SetResourceName(resource_name);
		resource->SetResourcePath(relative_path);

		if (!resource->LoadFromFile(relative_path))
		{
			return nullptr;
		}

		RegisterResource<T>(resource);

		return resource;
	}
}

template<typename T>
inline auto ResourceManager::GetResourceFromName(const std::string & name) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");

	for (const auto& resource : resource_groups[IResource::DeduceResourceType<T>()])
	{
		if(resource->GetResourceName()==name)
		return std::static_pointer_cast<T>(resource);
	}

	return nullptr;
}

template<typename T>
inline auto ResourceManager::GetResourceFromPath(const std::string & path) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");

	for (const auto& resource : resource_groups[IResource::DeduceResourceType<T>()])
	{
		if (resource->GetResourcePath() == path)
			return std::static_pointer_cast<T>(resource);
	}

	return nullptr;
}

template<typename T>
inline void ResourceManager::RegisterResource(std::shared_ptr<T>& resource)
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");

	if (!resource)
	{
		LOG_ERROR("resource is empty");
		return;
	}

	std::lock_guard<std::mutex> guard(resource_mutex);

	resource_groups[resource->GetResourceType()].emplace_back(resource);

}
