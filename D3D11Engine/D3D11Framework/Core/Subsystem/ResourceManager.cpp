#include "D3D11Framework.h"
#include "ResourceManager.h"
#include "Resource/Importer/TextureImporter.h"
#include "Resource/Importer/ModelImporter.h"

ResourceManager::ResourceManager(Context * context)
:ISubsystem(context)
{
   std::string root_directory=GetAssetDirectory();

   RegisterDirectory(AssetType::Shader, root_directory+"Shader/");
   RegisterDirectory(AssetType::Texture, root_directory + "Texture/");
   RegisterDirectory(AssetType::Icon, root_directory + "Icon/");
   RegisterDirectory(AssetType::Model, root_directory + "Model/");
   RegisterDirectory(AssetType::Audio, root_directory + "Audio/");
   RegisterDirectory(AssetType::Cubemap, root_directory + "Cubemap/");
   RegisterDirectory(AssetType::Font, root_directory + "Font/");
   RegisterDirectory(AssetType::Script, root_directory + "Script/");
   
   SetProjectDirectory("../../../_Project/");
}


const bool ResourceManager::Initialize()
{
	texture_importer=std::make_shared<TextureImporter>(context);
	model_importer = std::make_shared<ModelImporter>(context);

	return true;
}

auto ResourceManager::HasResource(const std::string & resource_name, const ResourceType & resource_type) -> const bool
{
	if (resource_name == NOT_ASSIGNED_STR)
	{
		LOG_ERROR("Invalid Parameter");
		return false;
	}

	for (const auto& resource : resource_groups[resource_type])
	{
		if(resource->GetResourceName()==resource_name)
		   return true;
	}

	return false;
}

void ResourceManager::SetProjectDirectory(const std::string &directory)
{
   if(!FileSystem::IsExistDirectory(directory))
	   FileSystem::Create_Directory(directory);

   project_directory=directory;
}

auto ResourceManager::GetAssetDirectory(const AssetType & type) -> const std::string
{
	for (const auto& directory : asset_directories)
	{
		if(directory.first==type)
		  return directory.second;
	}

	return NOT_ASSIGNED_STR;
}

void ResourceManager::RegisterDirectory(const AssetType & type, const std::string & directory)
{
    asset_directories[type]=directory;
}
