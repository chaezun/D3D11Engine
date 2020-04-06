#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Context * context)
:IResource(context, ResourceType::Mesh)
{
}

const bool Mesh::SaveToFile(const std::string & path)
{
	return false;
}

const bool Mesh::LoadFromFile(const std::string & path)
{
	return false;
}

void Mesh::SetMeshType(const MeshType & mesh_type)
{
	if (this->mesh_type == mesh_type)
		return;

   this->mesh_type=mesh_type;

   Clear();

   if(vertex_buffer)
     vertex_buffer->Clear();

   if (index_buffer)
	   index_buffer->Clear();
}

void Mesh::CreateBuffers(const D3D11_USAGE& usage)
{
    if(!vertex_buffer)
	  vertex_buffer=std::make_shared<VertexBuffer>(context);
    
	if(!index_buffer)
	   index_buffer=std::make_shared<IndexBuffer>(context);

	switch (mesh_type)
	{
	case MeshType::Cube:
		SetResourceName("Cube");
		break;
	case MeshType::Cylinder:
		SetResourceName("Cylinder");
		break;
	case MeshType::Cone:
		SetResourceName("Cone");
		break;
	case MeshType::Sphere:
		SetResourceName("Sphere");
		Geometry_Generator::CreateSphere(*this);
		break;
	case MeshType::Plane:
		SetResourceName("Plane");
		break;
	case MeshType::Capsule:
		SetResourceName("Capsule");
		break;
	case MeshType::Quad:
		SetResourceName("Quad");
		Geometry_Generator::CreateQuad(*this);
		break;
	default:
		break;
	}

	vertex_buffer->Create(vertices, usage);
	index_buffer->Create(indices, usage);
}
