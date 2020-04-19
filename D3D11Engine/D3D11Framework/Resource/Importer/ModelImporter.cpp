#include "D3D11Framework.h"
#include "ModelImporter.h"
#include "AssimpHelper.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"

#pragma warning(push,0)
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#pragma warning(pop)

ModelImporter::ModelImporter(Context * context)
	:context(context)
{
	scene_manager = context->GetSubsystem<SceneManager>();

	const auto major = aiGetVersionMajor();
	const auto minor = aiGetVersionMinor();
	const auto revision = aiGetVersionRevision();

	assimp_flags =
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_Debone |
		aiProcess_ConvertToLeftHanded;
}

/*
   aiProcess_CalcTangentSpace : Tangent 계산
   aiProcess_GenSmoothNormals : 자연스러운 normal을 보간
   aiProcess_JoinIdenticalVertices : 중복된 vertex를 하나로 합침
   aiProcess_OptimizeMeshes : mesh를 최적화(중복된 mesh를 줄여서 최적화)
   aiProcess_ImproveCacheLocality : 데이터간 공간이 비어있지 않게 데이터를 정렬함
   aiProcess_LimitBoneWeights : 뼈대에 가중치를 정해줌
   aiProcess_SplitLargeMeshes : 많은 개수의 mesh정보를 여러 버퍼에 분할에서 저장
   aiProcess_Triangulate : 삼각화
   aiProcess_GenUVCoords : UV좌표 계산
   aiProcess_SortByPType : Primitive Topology Type을 정렬
   aiProcess_FindDegenerates : 삼각화가 되어있지 않은 점과 선을 삭제
   aiProcess_FindInvalidData : 유효하지 않는 데이터를 찾음
   aiProcess_FindInstances : 중복된 mesh를 찾음
   aiProcess_ValidateDataStructure : 구조를 처음부터 쭉 검사
   aiProcess_Debone :
   aiProcess_ConvertToLeftHanded : 왼손 좌표계로 변환
*/

//모델 정보를 import
auto ModelImporter::Load(Model * model, const std::string & path) -> const bool 
{
	if (!model || !context)
	{
		LOG_ERROR("Invaild parameter");
		return false;
	}

	//ModelParameter 초기화
	//=====================================================================
	ModelParameter parameter;
	parameter.model = model;
	parameter.name = FileSystem::GetIntactFileNameFromPath(path);
	parameter.path = path;
	parameter.triangle_limit = 1'000'000;
	parameter.vertex_limit = 1'000'000;
	parameter.max_normal_smoothing_angle = 80.0f;
	parameter.max_tangent_smoothing_angle = 80.0f;
	//=====================================================================

	//Assimp Importer Setting
	//===================================================================================================================
	Assimp::Importer importer;
	//aiProcess_GenSmoothNormals 사용
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, parameter.max_normal_smoothing_angle);
	//aiProcess_CalcTangentSpace 사용
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, parameter.max_tangent_smoothing_angle);
	//aiProcess_SplitLargeMeshes 사용(Mesh)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, parameter.triangle_limit);
	//aiProcess_SplitLargeMeshes 사용(Vertext)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, parameter.vertex_limit);
	//점과 선은 사용하지 않음
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	//카메라와 조명 정보는 사용하지않음
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	//데이터를 불러오는데 걸리는 시간을 화면에 띄움
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
	//프로그래스바 연결
	importer.SetProgressHandler(new AssimpProgress(path));
	//로그 연결
	Assimp::DefaultLogger::set(new AssimpLogger());
	//===================================================================================================================

	//Assimp Importer에서 경로상의 파일을 읽어 aiScene을 반환
	//Assimp가 모델을 import하면 import된 모델 scene을 포함하고 있는 scene 객체를 불러옴
	if (const auto scene = importer.ReadFile(path, assimp_flags))
	{
		parameter.scene = scene;
		parameter.has_animation = scene->mNumAnimations != 0; //모델에 애니메이션 정보가 있는지 체크

		
		bool is_active = false;
		std::shared_ptr<Actor> new_actor = scene_manager->GetCurrentScene()->CreateActor(is_active);
		new_actor->SetName(parameter.name);
		new_actor->SetActive(true);
		parameter.model->SetRootActor(new_actor); //Root Actor 설정
		parameter.model->SetHasAnimation(parameter.has_animation); //Model의 애니메이션 설정

		//모델을 불러올 때 필요한 작업량
		int job_count = 0;
		AssimpHelper::ComputeNodeCount(scene->mRootNode, &job_count);
		ProgressReport::Get().SetJobCount(ProgressType::Model, job_count);

		ParseNodeHierarchy(scene->mRootNode);
		ParseNode(scene->mRootNode, parameter, nullptr, new_actor.get());
		ParseAnimations(parameter);
	}
	
	else
		LOG_ERROR_F("%s", importer.GetErrorString());

	//동적할당한 부분을 삭제
	importer.FreeScene();

	return parameter.scene != nullptr;
}

//scene에 저장되어있는 Root노드의 계층구조를 assimp_nodes vector에 추가
void ModelImporter::ParseNodeHierarchy(aiNode * assimp_node)
{
     assimp_nodes.emplace_back(assimp_node);

	 //자식 노드의 개수만큼 재귀함수 호출방식으로 vector에 추가
	 for (uint i = 0; i < assimp_node->mNumChildren; i++)
		 ParseNodeHierarchy(assimp_node->mChildren[i]);
}

void ModelImporter::ParseNode(const aiNode * assimp_node, const ModelParameter & parameter, Actor * parent_node, Actor * new_actor)
{
	//부모 노드가 있다면 자식 노드를 설정
	if (parent_node)
		new_actor->SetName(assimp_node->mName.C_Str());

	ProgressReport::Get().SetStatus(ProgressType::Model, "Creating Actor for " + new_actor->GetName());
	{
		const auto parent_transform = parent_node ? parent_node->GetTransform_Raw() : nullptr;
		new_actor->GetTransform_Raw()->SetParent(parent_transform);

		//각 노드들의 위치와 관련된 매트릭스를 구하고
		//local pos, rot, scal값을 설정
		AssimpHelper::ComputeActorTransform(assimp_node, new_actor);

		//노드의 mesh정보를 읽음
		ParseNodeMeshes(assimp_node, new_actor, parameter);

		//자식 노드의 개수만큼 재귀함수 호출방식으로 노드의 매트릭스를 구하고 local pos, rot, scal값을 설정
		for (uint i = 0; i < assimp_node->mNumChildren; i++)
		{
			auto child = scene_manager->GetCurrentScene()->CreateActor();
			ParseNode(assimp_node->mChildren[i], parameter, new_actor, child.get());
		}
	}
	ProgressReport::Get().IncrementJobsDone(ProgressType::Model);

}

void ModelImporter::ParseNodeMeshes(const aiNode * assimp_node, Actor * new_actor, const ModelParameter & parameter)
{
	//node의 mesh의 개수만큼 수행
	for (uint i = 0; i < assimp_node->mNumMeshes; i++)
	{
		auto actor = new_actor;
		const auto assimp_mesh = parameter.scene->mMeshes[assimp_node->mMeshes[i]];
		std::string name = assimp_node->mName.C_Str();

		if (assimp_node->mNumMeshes > 1)
		{
			auto is_active = false;
			actor = scene_manager->GetCurrentScene()->CreateActor(is_active).get();
			actor->GetTransform_Raw()->SetParent(new_actor->GetTransform_Raw());
			name += "_" + std::to_string(i + 1);
		}

		actor->SetName(name);
		LoadMesh(assimp_mesh, actor, parameter);
		actor->SetActive(true);
	}
}

void ModelImporter::ParseAnimations(const ModelParameter & parameter)
{
	//애니메이션 개수만큼 동작
	for (uint i = 0; i < parameter.scene->mNumAnimations; i++)
	{
		const auto assimp_animation = parameter.scene->mAnimations[i];
		auto animation = std::make_shared<Animation>(context);

		animation->SetName(assimp_animation->mName.C_Str());
		animation->SetDuration(assimp_animation->mDuration);
		animation->SetTicksPerSecond(assimp_animation->mTicksPerSecond != 0.0 ? assimp_animation->mTicksPerSecond : 25.0);

		for (uint j = 0; j < assimp_animation->mNumChannels; j++)
		{
			const auto assimp_animation_node = assimp_animation->mChannels[j];
			AnimationNode animation_node;

			animation_node.name = assimp_animation_node->mNodeName.C_Str();

			// Scale keys
			for (uint k = 0; k < assimp_animation_node->mNumScalingKeys; k++)
			{
				const auto time = assimp_animation_node->mScalingKeys[k].mTime;
				const auto value = AssimpHelper::ToVector3(assimp_animation_node->mScalingKeys[k].mValue);

				animation_node.scale_frames.emplace_back(KeyVector{ time, value }); //Uniform Initialization
			}

			// Position keys
			for (uint k = 0; k < assimp_animation_node->mNumPositionKeys; k++)
			{
				const auto time = assimp_animation_node->mPositionKeys[k].mTime;
				const auto value = AssimpHelper::ToVector3(assimp_animation_node->mPositionKeys[k].mValue);

				animation_node.position_frames.emplace_back(KeyVector{ time, value }); //Uniform Initialization
			}

			// Rotation keys
			for (uint k = 0; k < assimp_animation_node->mNumRotationKeys; k++)
			{
				const auto time = assimp_animation_node->mRotationKeys[k].mTime;
				const auto value = AssimpHelper::ToQuaternion(assimp_animation_node->mRotationKeys[k].mValue);

				animation_node.rotation_frames.emplace_back(KeyQuaternion{ time, value }); //Uniform Initialization
			}

			animation->AddAnimationNode(animation_node);
		}
		parameter.model->AddAnimation(animation->GetName(), animation);
	}
}

void ModelImporter::LoadMesh(aiMesh * assimp_mesh, Actor * parent_actor, const ModelParameter & parameter)
{
	if (!assimp_mesh || !parent_actor)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	const auto vertex_count = assimp_mesh->mNumVertices;

	std::vector<AnimationVertexWeights> vertex_weights;
	vertex_weights.reserve(vertex_count);
	vertex_weights.resize(vertex_count);

	LoadBone(assimp_mesh, parameter, vertex_weights);

	//Vertices
	std::vector<VertexTextureNormalTangent> vertices;
	{
		vertices.reserve(vertex_count);
		vertices.resize(vertex_count);

		for (uint i = 0; i < vertex_count; i++)
		{
			auto& vertex = vertices[i];

			if (assimp_mesh->mVertices)
				vertex.position = AssimpHelper::ToVector3(assimp_mesh->mVertices[i]);

			if (assimp_mesh->mNormals)
				vertex.normal = AssimpHelper::ToVector3(assimp_mesh->mNormals[i]);

			if (assimp_mesh->mTangents)
				vertex.tangent = AssimpHelper::ToVector3(assimp_mesh->mTangents[i]);

			const uint uv_channel = 0;
			if (assimp_mesh->HasTextureCoords(uv_channel))
				vertex.uv = AssimpHelper::ToVector2(assimp_mesh->mTextureCoords[uv_channel][i]);

			if (!vertex_weights.empty())
			{
				vertex.indices.x = static_cast<float>(vertex_weights[i].bone_ids[0]);
				vertex.indices.y = static_cast<float>(vertex_weights[i].bone_ids[1]);
				vertex.indices.z = static_cast<float>(vertex_weights[i].bone_ids[2]);
				vertex.indices.w = static_cast<float>(vertex_weights[i].bone_ids[3]);

				vertex.weights.x = vertex_weights[i].weights[0];
				vertex.weights.y = vertex_weights[i].weights[1];
				vertex.weights.z = vertex_weights[i].weights[2];
				vertex.weights.w = vertex_weights[i].weights[3];
			}
		}
	}

	//Indices
	std::vector<uint> indices;
	{
		const auto index_count = assimp_mesh->mNumFaces * 3;
		indices.reserve(index_count);
		indices.resize(index_count);

		for (uint i = 0; i < assimp_mesh->mNumFaces; i++)
		{
			auto& face = assimp_mesh->mFaces[i];
			indices[i * 3 + 0] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}
	}

	auto renderable = parent_actor->AddComponent<Renderable>();
	renderable->SetHasAnimation(parameter.has_animation);

	parameter.model->AddMesh(vertices, indices, renderable);

	if (parameter.scene->HasMaterials())
	{
		const auto assimp_material = parameter.scene->mMaterials[assimp_mesh->mMaterialIndex];
		parameter.model->AddMaterial(LoadMaterial(assimp_material, parameter), renderable);
	}
}

void ModelImporter::LoadBone(const aiMesh * assimp_mesh, const ModelParameter & parameter, std::vector<AnimationVertexWeights>& vertex_weights)
{
	for (int i = static_cast<int>(assimp_mesh->mNumBones - 1); i >= 0; i--)
	{
		uint bone_index = 0;
		std::string name = assimp_mesh->mBones[i]->mName.data;

		if (bone_mapping.find(name) == bone_mapping.end())
		{
			bone_index = bone_count;
			bone_count++;

			auto node = FindAssimpNode(name);
			auto parent_index = parameter.model->FindBoneIndex(node->mParent->mName.data);

			bone_mapping[name] = bone_index;
			parameter.model->AddBone(name, parent_index, AssimpHelper::ToMatrix(assimp_mesh->mBones[i]->mOffsetMatrix));
		}
		else
			bone_index = bone_mapping[name];

		for (uint j = 0; j < assimp_mesh->mBones[i]->mNumWeights; j++)
		{
			uint vertex_id = assimp_mesh->mBones[i]->mWeights[j].mVertexId;
			vertex_weights[vertex_id].AddData(bone_index, assimp_mesh->mBones[i]->mWeights[j].mWeight);
		}
	}
}

auto ModelImporter::LoadMaterial(aiMaterial * assimp_material, const ModelParameter & parameter) -> std::shared_ptr<class Material>
{
	if (!assimp_material)
	{
		LOG_ERROR("Invalid parameter");
		return nullptr;
	}

	auto material = std::make_shared<Material>(context);

	//Name
	aiString material_name;
	//material_name에 AI_MATKEY_NAME값을 통해 이름을 저장
	aiGetMaterialString(assimp_material, AI_MATKEY_NAME, &material_name);

	//Cull Mode
	//양면의 개수가 들어갈 변수
	int is_two_sided = 0;
	uint max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(assimp_material, AI_MATKEY_TWOSIDED, &is_two_sided, &max))
	{
		//양면을 사용하는 경우 (양면이 1개 이상인 경우)
		if (is_two_sided != 0)
			material->SetCullMode(D3D11_CULL_NONE);
	}

	//Diffuse Color
	aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(assimp_material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);

	//Opacity
	//opacity안의 r,g,b,a값은 모두 같은 투명도값을 가짐
	aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(assimp_material, AI_MATKEY_OPACITY, &opacity);

	material->SetAlbedoColor(Color4(diffuse_color.r, diffuse_color.g, diffuse_color.b, opacity.r));

	const auto load_texture = [&parameter, &assimp_material, &material, this](const aiTextureType& assimp_type, const TextureType& native_type)
	{
		aiString texture_path;
		if (assimp_material->GetTextureCount(assimp_type) > 0)
		{
			if (AI_SUCCESS == assimp_material->GetTexture(assimp_type, 0, &texture_path))
			{
				const auto deduced_path = AssimpHelper::ValidatePath(texture_path.data, parameter.path);
				if (FileSystem::IsSupportedTextureFile(deduced_path))
					parameter.model->AddTexture(material, native_type, deduced_path);

				//embedded_texture : 모델에 포함된 텍스처(이미지) 파일이 있는지 확인
				else if (const auto embedded_texture = parameter.scene->GetEmbeddedTexture(FileSystem::GetFileNameFromPath(texture_path.data).c_str()))
				{
					const auto path = Texture2D::SaveEmbeddedTextureToFile
					(
						parameter.model->GetTextureDirectory() + FileSystem::GetFileNameFromPath(embedded_texture->mFilename.data),
						embedded_texture->achFormatHint,
						embedded_texture->mWidth,
						embedded_texture->mHeight,
						embedded_texture->pcData
					);

					//if (path != NOT_ASSIGNED_STR)
					//    model->AddTexture(material, native_type, path);
				}

				if (assimp_type == aiTextureType_DIFFUSE)
					material->SetAlbedoColor(Color4::White);

				if (native_type == TextureType::Normal || native_type == TextureType::Height)
				{
					if (const auto texture = material->GetTexture(native_type))
					{
						auto property_type = native_type;
						//Height
						property_type = (property_type == TextureType::Normal && texture->IsGrayscale()) ? TextureType::Height : property_type;
						//Normal
						property_type = (property_type == TextureType::Height && !texture->IsGrayscale()) ? TextureType::Normal : property_type;

						//위에서 걸려서 property_type의 값이 변경되었을때 정상적인 texture로 다시 material에 저장
						if (property_type != native_type)
						{
							material->SetTexture(native_type, std::shared_ptr<ITexture>());
							material->SetTexture(property_type, texture);
						}
					}
					else
						LOG_ERROR("Failed to get texture");
				}
			}
		}
	};

	load_texture(aiTextureType_DIFFUSE, TextureType::Albedo);
	//SHININESS : 반사되는 빛의 강도
	//load_texture(aiTextureType_SHININESS, TextureType::Roughness);
	//AMBIENT : 자신이 가지고 있는 고유 색
	//load_texture(aiTextureType_AMBIENT, TextureType::Metallic);
	load_texture(aiTextureType_NORMALS, TextureType::Normal);
	//Occlusion: 방향성을 가지지 않는 그림자
	//load_texture(aiTextureType_LIGHTMAP, TextureType::Occlusion);
	//load_texture(aiTextureType_EMISSIVE, TextureType::Emissive);
	load_texture(aiTextureType_HEIGHT, TextureType::Height);
	//load_texture(aiTextureType_OPACITY, TextureType::Mask);

	return material;
}

auto ModelImporter::FindAssimpNode(const std::string & node_name) -> struct aiNode *
{
	for (uint i = 0; i < assimp_nodes.size(); i++)
	{
		if (assimp_nodes[i]->mName.data == node_name)
			return assimp_nodes[i];
	}

	return nullptr;
}