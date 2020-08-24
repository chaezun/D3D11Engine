#pragma once
#include "IComponent.h"

//Rendering에 관련된 Mesh, Material, BoundBox, Animation 등을 등록하는 클래스
class Renderable final : public IComponent
{
public:
	Renderable
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Renderable()=default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//=================================================================================================
	// [Material]
	//=================================================================================================
	auto GetMaterial() const -> const std::shared_ptr<Material>& {return material;}
	void SetMaterial(const std::shared_ptr<Material>& material) {this->material=material;}
	void SetMaterial(const std::string& path);
	void SetStandardMaterial();


	//=================================================================================================
	// [Mesh]
	//=================================================================================================
	auto GetMesh() const -> const std::shared_ptr<Mesh>& { return mesh; }
	void SetMesh(const std::shared_ptr<Mesh>& mesh) { this->mesh = mesh; }
	void SetMesh(const std::string& path);
	void SetMesh(const std::string& name, const std::vector<struct VertexTextureNormalTangent>& vertices, const std::vector<uint>& indices, const D3D11_USAGE& usage = D3D11_USAGE_IMMUTABLE);
	void SetStandardMesh(const MeshType& mesh_type);

	//=================================================================================================
	// [BoundBox]
	//=================================================================================================
	auto GetBoundBox() -> const BoundBox;
	void SetBoundBox(const BoundBox& aabb) { this->aabb = aabb; }

	//=================================================================================================
	// [Animation]
	//=================================================================================================
	auto GetHasAnimation() const { return has_animation; }
	void SetHasAnimation(const bool& has_animation) { this->has_animation = has_animation; }

private:
   std::shared_ptr<Material> material;
   std::shared_ptr<Mesh> mesh;
   BoundBox aabb;
   bool has_animation = false;
}; 