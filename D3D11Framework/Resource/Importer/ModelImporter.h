#pragma once

//Assimp : 모델 Importing 라이브러리(Open Asset Import Library)
//aiScene객체에는 모델의 material, mesh들이 포함되어 있음.
struct ModelParameter final
{
	const struct aiScene* scene = nullptr;
	class Model* model = nullptr;
	std::shared_ptr<Renderable> renderable ;
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

	//애니메이션 정보를 제외한 나머지 정보를 추출하는 함수
	auto Load(class Model* model, const std::string& path) -> const bool;
	//LoadAnimation은 Load가 선행되고 실행시켜야하는 함수
	//애니메이션 정보를 추출하는 함수
	auto LoadAnimation(class Model* model, const std::string& path) -> const bool;

private:
	//Parsing
	void ParseNodeHierarchy(struct aiNode* assimp_node);
	void ParseNode(const struct aiNode* assimp_node, const ModelParameter& parameter, class Actor* parent_node = nullptr, class Actor* new_actor = nullptr);
	void ParseNodeMeshes(const struct aiNode* assimp_node, class Actor* new_actor, const ModelParameter& parameter);
	void ParseAnimations(const ModelParameter& parameter, const std::string& animation_name);

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