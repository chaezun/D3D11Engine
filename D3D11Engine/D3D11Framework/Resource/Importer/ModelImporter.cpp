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
   aiProcess_CalcTangentSpace : Tangent ���
   aiProcess_GenSmoothNormals : �ڿ������� normal�� ����
   aiProcess_JoinIdenticalVertices : �ߺ��� vertex�� �ϳ��� ��ħ
   aiProcess_OptimizeMeshes : mesh�� ����ȭ(�ߺ��� mesh�� �ٿ��� ����ȭ)
   aiProcess_ImproveCacheLocality : �����Ͱ� ������ ������� �ʰ� �����͸� ������
   aiProcess_LimitBoneWeights : ���뿡 ����ġ�� ������
   aiProcess_SplitLargeMeshes : ���� ������ mesh������ ���� ���ۿ� ���ҿ��� ����
   aiProcess_Triangulate : �ﰢȭ
   aiProcess_GenUVCoords : UV��ǥ ���
   aiProcess_SortByPType : Primitive Topology Type�� ����
   aiProcess_FindDegenerates : �ﰢȭ�� �Ǿ����� ���� ���� ���� ����
   aiProcess_FindInvalidData : ��ȿ���� �ʴ� �����͸� ã��
   aiProcess_FindInstances : �ߺ��� mesh�� ã��
   aiProcess_ValidateDataStructure : ������ ó������ �� �˻�
   aiProcess_Debone :
   aiProcess_ConvertToLeftHanded : �޼� ��ǥ��� ��ȯ
*/

//�� ������ import
auto ModelImporter::Load(Model * model, const std::string & path) -> const bool 
{
	if (!model || !context)
	{
		LOG_ERROR("Invaild parameter");
		return false;
	}

	//ModelParameter �ʱ�ȭ
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
	//aiProcess_GenSmoothNormals ���
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, parameter.max_normal_smoothing_angle);
	//aiProcess_CalcTangentSpace ���
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, parameter.max_tangent_smoothing_angle);
	//aiProcess_SplitLargeMeshes ���(Mesh)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, parameter.triangle_limit);
	//aiProcess_SplitLargeMeshes ���(Vertext)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, parameter.vertex_limit);
	//���� ���� ������� ����
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	//ī�޶�� ���� ������ �����������
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	//�����͸� �ҷ����µ� �ɸ��� �ð��� ȭ�鿡 ���
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
	//���α׷����� ����
	importer.SetProgressHandler(new AssimpProgress(path));
	//�α� ����
	Assimp::DefaultLogger::set(new AssimpLogger());
	//===================================================================================================================

	//Assimp Importer���� ��λ��� ������ �о� aiScene�� ��ȯ
	//Assimp�� ���� import�ϸ� import�� �� scene�� �����ϰ� �ִ� scene ��ü�� �ҷ���
	if (const auto scene = importer.ReadFile(path, assimp_flags))
	{
		parameter.scene = scene;
		parameter.has_animation = scene->mNumAnimations != 0; //�𵨿� �ִϸ��̼� ������ �ִ��� üũ

		
		bool is_active = false;
		std::shared_ptr<Actor> new_actor = scene_manager->GetCurrentScene()->CreateActor(is_active);
		new_actor->SetName(parameter.name);
		new_actor->SetActive(true);
		parameter.model->SetRootActor(new_actor); //Root Actor ����
		parameter.model->SetHasAnimation(parameter.has_animation); //Model�� �ִϸ��̼� ����

		//���� �ҷ��� �� �ʿ��� �۾���
		int job_count = 0;
		AssimpHelper::ComputeNodeCount(scene->mRootNode, &job_count);
		ProgressReport::Get().SetJobCount(ProgressType::Model, job_count);

		ParseNodeHierarchy(scene->mRootNode);
		ParseNode(scene->mRootNode, parameter, nullptr, new_actor.get());
		ParseAnimations(parameter);
	}
	
	else
		LOG_ERROR_F("%s", importer.GetErrorString());

	//�����Ҵ��� �κ��� ����
	importer.FreeScene();

	return parameter.scene != nullptr;
}

//scene�� ����Ǿ��ִ� Root����� ���������� assimp_nodes vector�� �߰�
void ModelImporter::ParseNodeHierarchy(aiNode * assimp_node)
{
     assimp_nodes.emplace_back(assimp_node);

	 //�ڽ� ����� ������ŭ ����Լ� ȣ�������� vector�� �߰�
	 for (uint i = 0; i < assimp_node->mNumChildren; i++)
		 ParseNodeHierarchy(assimp_node->mChildren[i]);
}

void ModelImporter::ParseNode(const aiNode * assimp_node, const ModelParameter & parameter, Actor * parent_node, Actor * new_actor)
{
	//�θ� ��尡 �ִٸ� �ڽ� ��带 ����
	if (parent_node)
		new_actor->SetName(assimp_node->mName.C_Str());

	ProgressReport::Get().SetStatus(ProgressType::Model, "Creating Actor for " + new_actor->GetName());
	{
		const auto parent_transform = parent_node ? parent_node->GetTransform_Raw() : nullptr;
		new_actor->GetTransform_Raw()->SetParent(parent_transform);

		//�� ������ ��ġ�� ���õ� ��Ʈ������ ���ϰ�
		//local pos, rot, scal���� ����
		AssimpHelper::ComputeActorTransform(assimp_node, new_actor);

		//����� mesh������ ����
		ParseNodeMeshes(assimp_node, new_actor, parameter);

		//�ڽ� ����� ������ŭ ����Լ� ȣ�������� ����� ��Ʈ������ ���ϰ� local pos, rot, scal���� ����
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
	//node�� mesh�� ������ŭ ����
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
	//�ִϸ��̼� ������ŭ ����
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
	//material_name�� AI_MATKEY_NAME���� ���� �̸��� ����
	aiGetMaterialString(assimp_material, AI_MATKEY_NAME, &material_name);

	//Cull Mode
	//����� ������ �� ����
	int is_two_sided = 0;
	uint max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(assimp_material, AI_MATKEY_TWOSIDED, &is_two_sided, &max))
	{
		//����� ����ϴ� ��� (����� 1�� �̻��� ���)
		if (is_two_sided != 0)
			material->SetCullMode(D3D11_CULL_NONE);
	}

	//Diffuse Color
	aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(assimp_material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);

	//Opacity
	//opacity���� r,g,b,a���� ��� ���� �������� ����
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

				//embedded_texture : �𵨿� ���Ե� �ؽ�ó(�̹���) ������ �ִ��� Ȯ��
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

						//������ �ɷ��� property_type�� ���� ����Ǿ����� �������� texture�� �ٽ� material�� ����
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
	//SHININESS : �ݻ�Ǵ� ���� ����
	//load_texture(aiTextureType_SHININESS, TextureType::Roughness);
	//AMBIENT : �ڽ��� ������ �ִ� ���� ��
	//load_texture(aiTextureType_AMBIENT, TextureType::Metallic);
	load_texture(aiTextureType_NORMALS, TextureType::Normal);
	//Occlusion: ���⼺�� ������ �ʴ� �׸���
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