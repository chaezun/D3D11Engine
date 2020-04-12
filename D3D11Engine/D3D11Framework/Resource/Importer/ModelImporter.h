#pragma once

struct ModelParameter final
{
	const struct aiScene* scene = nullptr;
	class Model* model = nullptr;
	std::string path = "";
	std::string name = "";
	uint triangle_limit = 0;
	uint vertex_limit = 0;
	float max_normal_smoothing_angle = 0.0f;
	float max_tangent_smoothing_angle = 0.0f;
	bool has_animation = false;
};

class ModelImporter final
{
public:
	ModelImporter(class Context* context);
	~ModelImporter() = default;

	auto Load(class Model* model, const std::string& path) -> const bool;

private:
    void ReadNodeHierarchy
	(
	   const struct aiScene* assimp_scene,
	   struct aiNode* assimp_node,
	   class Model* model,
	   class Actor* parent_actor=nullptr,
	   class Actor* new_actor=nullptr
	);

	void LoadMesh
	(
	   const struct aiScene* assimp_scene,
	   struct aiMesh* assimp_mesh,
	   class Model* model,
	   class Actor* actor
	);

	auto LoadMaterial
	(
	  const struct aiScene* assimp_scene,
	  struct aiMaterial* assimp_material,
	  class Model* model
	) -> std::shared_ptr<Material>;

private:
   class Context* context            = nullptr;
   class SceneManager* scene_manager = nullptr;
  
   int assimp_flags                  = 0;
};