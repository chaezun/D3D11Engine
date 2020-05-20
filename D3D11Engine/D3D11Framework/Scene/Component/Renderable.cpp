#include "D3D11Framework.h"
#include "Renderable.h"
#include "Transform.h"

Renderable::Renderable(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
}

void Renderable::OnStart()
{
}

void Renderable::OnUpdate()
{
}

void Renderable::OnStop()
{
}

void Renderable::SetMaterial(const std::string & path)
{
	material = context->GetSubsystem<ResourceManager>()->Load<Material>(path);
}

void Renderable::SetStandardMaterial()
{
	if (auto resource = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Material>("Standard"))
	{
		material = resource;
		return;
	}

	auto resource_manager = context->GetSubsystem<ResourceManager>();

	material = std::make_shared<Material>(context);
	material->SetAlbedoColor(Color4(0.6f, 0.6f, 0.6f, 1.0f));
	material->SetStandardShader();
	material->SetResourceName("Standard");
	context->GetSubsystem<ResourceManager>()->RegisterResource(material);
}

void Renderable::SetMesh(const std::string & path)
{
	mesh = context->GetSubsystem<ResourceManager>()->Load<Mesh>(path);
}

void Renderable::SetMesh(const std::string & name, const std::vector<struct VertexTextureNormalTangent>& vertices, const std::vector<uint>& indices, const D3D11_USAGE& usage)
{
	mesh = std::make_shared<Mesh>(context);
	mesh->SetMeshType(MeshType::Custom);
	mesh->SetResourceName(name);
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);
	mesh->CreateBuffers(usage);

	context->GetSubsystem<ResourceManager>()->RegisterResource(mesh);
}

void Renderable::SetStandardMesh(const MeshType & mesh_type)
{
	std::string mesh_name = "";
	switch (mesh_type)
	{
	case MeshType::Cube:        mesh_name = "Cube";     break;
	case MeshType::Cylinder:    mesh_name = "Cylinder"; break;
	case MeshType::Cone:        mesh_name = "Cone";     break;
	case MeshType::Sphere:      mesh_name = "Sphere";   break;
	case MeshType::Plane:       mesh_name = "Plane";    break;
	case MeshType::Capsule:     mesh_name = "Capsule";  break;
	case MeshType::Quad:        mesh_name = "Quad";     break;
	}

	if (auto resource = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Mesh>("Standard_" + mesh_name))
	{
		mesh = resource;
		return;
	}

	mesh = std::make_shared<Mesh>(context);
	mesh->SetMeshType(mesh_type);
	mesh->CreateBuffers();
	mesh->SetResourceName("Standard_" + mesh_name);
}

auto Renderable::GetBoundBox() -> const BoundBox
{
	return aabb.Transformed(transform->GetWorldMatrix());
}
