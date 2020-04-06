#include "Framework.h"
#include "Model.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Importer/ModelImporter.h"

Model::Model(Context * context)
    :IResource(context, ResourceType::Model)
{
}

Model::~Model()
{
    meshes.clear();
    meshes.shrink_to_fit();

    materials.clear();
    materials.shrink_to_fit();
}

const bool Model::SaveToFile(const std::string & path)
{
    return false;
}

const bool Model::LoadFromFile(const std::string & path)
{
    auto resource_manager = context->GetSubsystem<ResourceManager>();

    SetWorkingDirectories(resource_manager->GetProjectDirectory() + FileSystem::GetIntactFileNameFromPath(path));
    SetResourcePath(model_directory + FileSystem::GetIntactFileNameFromPath(path) + MODEL_BIN_EXTENSION);
    SetResourceName(FileSystem::GetIntactFileNameFromPath(path));

    if (resource_manager->GetModelImporter()->Load(this, path))
    {
		normalize_scale = ComputeNormalizedScale();
		root_actor->GetTransform()->SetScale(normalize_scale);
		root_actor->GetTransform()->UpdateTransform();

        SaveToFile(resource_path);
        return true;
    }

    return false;
}

auto Model::GetMaterial(const uint & index) -> std::shared_ptr<Material>
{
    if (index >= materials.size())
    {
        LOG_ERROR("Invalid parameter, out of range");
        return nullptr;
    }

    return materials[index];
}

void Model::AddMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<class Renderable>& renderable)
{
    if (!material || !renderable)
    {
        LOG_ERROR("Invalid parameter");
        return;
    }

    material->SetResourcePath(material_directory + material->GetResourceName() + MATERIAL_BIN_EXTENSION);
    material->SaveToFile(material->GetResourcePath());
    materials.emplace_back(material);

    renderable->SetMaterial(material);
}

void Model::AddTexture(const std::shared_ptr<Material>& material, const TextureType & texture_type, const std::string & path)
{
    if (!material || path == NOT_ASSIGNED_STR)
    {
        LOG_ERROR("Invalid parameter");
        return;
    }

    const auto texture_name = FileSystem::GetIntactFileNameFromPath(path);
    auto texture = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Texture2D>(texture_name);

    if (texture)
        material->SetTexture(texture_type, texture);
    else
    {
        texture = std::make_shared<Texture2D>(context);
        texture->LoadFromFile(path);
        texture->SetResourcePath(texture_directory + texture_name + TEXTURE_BIN_EXTENSION);
        texture->SetResourceName(texture_name);
        texture->SaveToFile(texture->GetResourcePath());
        texture->ClearMipChain();

        material->SetTexture(texture_type, texture);
    }
}

auto Model::GetMesh(const uint & index) -> std::shared_ptr<Mesh>
{
    if (index >= meshes.size())
    {
        LOG_ERROR("Invalid parameter, out of range");
        return nullptr;
    }

    return meshes[index];
}

void Model::AddMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<class Renderable>& renderable)
{
    if (!mesh || !renderable)
    {
        LOG_ERROR("Invalid parameter");
        return;
    }

	bound_box = BoundBox(mesh->GetVertices());

    mesh->SetResourcePath(mesh_directory + mesh->GetResourceName() + MESH_BIN_EXTENSION);
    mesh->SaveToFile(mesh->GetResourcePath());
    meshes.emplace_back(mesh);

    renderable->SetMesh(mesh);
	renderable->SetBoundBox(bound_box);
}

void Model::AddMesh(const std::vector<VertexTextureNormalTangent>& vertices, const std::vector<uint>& indices, const std::shared_ptr<class Renderable>& renderable)
{
    if (vertices.empty() || indices.empty() || !renderable)
    {
        LOG_ERROR("Invalid parameter");
        return;
    }

    auto mesh = std::make_shared<Mesh>(context);
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->CreateBuffers();
    //TODO: mesh name


    AddMesh(mesh, renderable);
}

void Model::SetWorkingDirectories(const std::string & directory)
{
    model_directory     = directory + "/";
    material_directory  = model_directory + "Material/";
    mesh_directory      = model_directory + "Mesh/";
    texture_directory   = model_directory + "Texture/";

    FileSystem::Create_Directory(model_directory);
    FileSystem::Create_Directory(material_directory);
    FileSystem::Create_Directory(mesh_directory);
    FileSystem::Create_Directory(texture_directory);
}

auto Model::ComputeNormalizedScale() const -> const float
{
	const auto scale = bound_box.GetExtents().Length();

	return 1.0f / scale;
}
