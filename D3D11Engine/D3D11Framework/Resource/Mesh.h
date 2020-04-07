#pragma once
#include "IResource.h"

enum class MeshType : uint
{
	Custom,
	Cube,
	Cylinder,
	Cone,
	Sphere,
	Plane,
	Capsule,
	Quad,
};

class Mesh final : public IResource, public Geometry<VertexTextureNormalTangent>
{
public: 
	Mesh(class Context* context);
	~Mesh()=default;

    const bool SaveToFile(const std::string& path)  override;
    const bool LoadFromFile(const std::string& path) override;

	auto GetMeshType() const -> const MeshType& {return mesh_type;}
	void SetMeshType(const MeshType& mesh_type);

	auto GetPrimitiveTopology() const -> const D3D11_PRIMITIVE_TOPOLOGY& {return primitive_topology;}
	void SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY& primitive_topology) { this->primitive_topology= primitive_topology;}

	auto GetVertexBuffer() const -> const std::shared_ptr<VertexBuffer>& {return vertex_buffer;}
	auto GetIndexBuffer() const -> const std::shared_ptr<IndexBuffer>& { return index_buffer; }

	void CreateBuffers(const D3D11_USAGE& usage = D3D11_USAGE_IMMUTABLE);

private:
  D3D11_PRIMITIVE_TOPOLOGY primitive_topology=D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  MeshType mesh_type=MeshType::Custom;
  std::shared_ptr<VertexBuffer> vertex_buffer;
  std::shared_ptr<IndexBuffer> index_buffer;
};