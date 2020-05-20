#pragma once

//aiScene객체에는 모델의 material, mesh들이 포함되어 있음.
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
	//Parsing
	void ParseNodeHierarchy(struct aiNode* assimp_node);
	void ParseNode(const struct aiNode* assimp_node, const ModelParameter& parameter, class Actor* parent_node = nullptr, class Actor* new_actor = nullptr);
	void ParseNodeMeshes(const struct aiNode* assimp_node, class Actor* new_actor, const ModelParameter& parameter);
	void ParseAnimations(const ModelParameter& parameter);

	//Loading
	void LoadMesh(struct aiMesh* assimp_mesh, class Actor* parent_actor, const ModelParameter& parameter);
	void LoadBone(const struct aiMesh* assimp_mesh, const ModelParameter& parameter, std::vector<AnimationVertexWeights>& vertex_weights);
	auto LoadMaterial(struct aiMaterial* assimp_material, const ModelParameter& parameter)->std::shared_ptr<class Material>;

private:
	auto FindAssimpNode(const std::string& node_name) -> struct aiNode*;

private:
   class Context* context            = nullptr;
   class SceneManager* scene_manager = nullptr;
  
   int assimp_flags                  = 0;
   Matrix global_inverse_transform = Matrix::Identity;
   uint bone_count = 0;
   std::vector<struct aiNode*> assimp_nodes;
   std::map<std::string, uint> bone_mapping;
};