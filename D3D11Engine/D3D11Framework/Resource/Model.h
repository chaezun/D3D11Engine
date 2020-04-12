#pragma once
#include "IResource.h"

class Model final : public IResource
{
public:
	Model(class Context* context);
	~Model();

	const bool SaveToFile(const std::string& path)  override;
	const bool LoadFromFile(const std::string& path) override;

	//=================================================================================================
	// [Actor]
	//=================================================================================================
	auto GetRootActor() const -> const std::shared_ptr<class Actor>& { return root_actor; }
	void SetRootActor(const std::shared_ptr<class Actor>& actor)  { root_actor=actor; }

	//=================================================================================================
	// [Material]
	//=================================================================================================
	auto GetMaterials() const -> const std::vector<std::shared_ptr<Material>>& { return materials; }
	auto GetMaterial(const uint& index = 0)->std::shared_ptr<Material>;
	void AddMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<class Renderable>& renderable);
	void AddTexture(const std::shared_ptr<Material>& material, const TextureType& texture_type, const std::string& path);

	//=================================================================================================
	// [Mesh]
	//=================================================================================================
	auto GetMeshes() const -> const std::vector<std::shared_ptr<Mesh>>& {return meshes;}
	auto GetMesh(const uint& index=0) -> std::shared_ptr<Mesh>;
	void AddMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<class Renderable>& renderable);
    void AddMesh(const std::vector<VertexTextureNormalTangent>& vertices, const std::vector<uint>& indices, const std::shared_ptr<class Renderable>& renderable);

	//=====================================================================
    // [Skeleton]
    //=====================================================================
	auto GetSkeleton() const -> const std::shared_ptr<Skeleton>& { return skeleton; }
	void AddBone(const std::string& name, const int& parent_index, const Matrix& offset);
	void AddBone(const Bone& bone);
	auto FindBone(const uint& bone_index) const->Bone*;
	auto FindBone(const std::string& name) const->Bone*;
	auto FindBoneIndex(const std::string& name) const -> const int;

	//=====================================================================
	// [Animation]
	//=====================================================================
	auto GetAnimations() const -> const std::map<std::string, std::shared_ptr<Animation>>& { return animations; }
	void AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation);
	auto FindAnimation(const std::string& name) -> const std::shared_ptr<Animation>;
	auto IsAnimated() const { return is_animated; }
	void SetAnimated(const bool& is_animated) { this->is_animated = is_animated; }

	//=================================================================================================
	// [Directory]
	//=================================================================================================
	auto GetModelDirectory() const -> const std::string& {return model_directory;}
	auto GetMeshDirectory() const -> const std::string& { return mesh_directory; }
	auto GetMaterialDirectory() const -> const std::string& { return material_directory; }
	auto GetTextureDirectory() const -> const std::string& { return texture_directory; }
	auto GetAnimationDirectory() const -> const std::string& { return animation_directory; }

private:
	void SetWorkingDirectories(const std::string& directory);
	auto ComputeNormalizedScale()const->const float;

private:
    std::shared_ptr<class Actor> root_actor;

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::map<std::string, std::shared_ptr<Animation>> animations;
	std::shared_ptr<Skeleton> skeleton;

    std::string model_directory="";
	std::string mesh_directory = "";
	std::string material_directory = "";
	std::string texture_directory = "";
	std::string animation_directory = "";

	BoundBox bound_box;
	float normalize_scale          =1.0f;
	bool is_animated               =false;
};