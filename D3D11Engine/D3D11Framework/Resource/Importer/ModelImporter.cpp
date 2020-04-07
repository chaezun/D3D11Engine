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

ModelImporter::~ModelImporter()
{

}

auto ModelImporter::Load(Model * model, const std::string & path) -> const bool
{
	model_path = FileSystem::GetRelativeFromPath(path);

	Assimp::Importer importer;
	//aiProcess_GenSmoothNormals ���
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	//aiProcess_CalcTangentSpace ���
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	//aiProcess_SplitLargeMeshes ���(Mesh)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	//aiProcess_SplitLargeMeshes ���(Vertext)
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000);
	//���� ���� ������� ����
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	//ī�޶�� ���� ������ �����������
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	//�����͸� �ҷ����µ� �ɸ��� �ð��� ȭ�鿡 ���
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
	//���α׷����� ����
	importer.SetProgressHandler(new AssimpProgress(model_path));
	//�α� ����
	Assimp::DefaultLogger::set(new AssimpLogger());

	const auto scene = importer.ReadFile(model_path, assimp_flags);
	const auto result = scene != nullptr;

	if (result)
		ReadNodeHierarchy(scene, scene->mRootNode, model);

	else
		LOG_ERROR_F("%s", importer.GetErrorString());

	//�����Ҵ��� �κ��� ����
	importer.FreeScene();

	return result;
}

void ModelImporter::ReadNodeHierarchy(const aiScene * assimp_scene, aiNode * assimp_node, Model * model, Actor * parent_actor, Actor * new_actor)
{
	if (!assimp_node->mParent || !new_actor)
	{
		new_actor = scene_manager->GetCurrentScene()->CreateActor().get();
		model->SetRootActor(new_actor->shared_from_this());

		int job_count = 0;
		AssimpHelper::ComputeNodeCount(assimp_node, &job_count);
		ProgressReport::Get().SetJobCount(ProgressType::Model, job_count);
	}

	const auto actor_name = assimp_node->mParent ? assimp_node->mName.C_Str() : FileSystem::GetIntactFileNameFromPath(model_path);
	new_actor->SetName(actor_name);

	ProgressReport::Get().SetStatus(ProgressType::Model, "Creating model for " + actor_name);
	{
		const auto parent_transform = parent_actor ? parent_actor->GetTransform() : nullptr;
		new_actor->GetTransform()->SetParent(parent_transform.get());
		//�� ��帶�� tranform �����͸� ����
		AssimpHelper::ComputeActorTransform(assimp_node, new_actor);

		for (uint i = 0; i < assimp_node->mNumMeshes; i++)
		{
			auto actor = new_actor;
			const auto assimp_mesh = assimp_scene->mMeshes[assimp_node->mMeshes[i]];
			std::string mesh_name = assimp_node->mName.C_Str();

			if (assimp_node->mNumMeshes > 1)
			{
				actor = scene_manager->GetCurrentScene()->CreateActor().get();
				actor->GetTransform()->SetParent(new_actor->GetTransform().get());
				mesh_name += "_" + std::to_string(i + 1);
			}

			actor->SetName(mesh_name);

			LoadMesh(assimp_scene, assimp_mesh, model, actor);
		}

		for (uint i = 0; i < assimp_node->mNumChildren; i++)
		{
			auto child = scene_manager->GetCurrentScene()->CreateActor().get();
			//��ͷ� ��� �ڽ� ��忡�� �����͸� ����
			ReadNodeHierarchy(assimp_scene, assimp_node->mChildren[i], model, new_actor, child);
		}
	}
	ProgressReport::Get().IncrementJobsDone(ProgressType::Model);
}

void ModelImporter::LoadMesh(const aiScene * assimp_scene, aiMesh * assimp_mesh, Model * model, Actor * actor)
{
	if (!model || !assimp_scene || !assimp_mesh || !actor)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	//Vertices
	std::vector<VertexTextureNormalTangent> vertices;
	{
		const auto vertex_count = assimp_mesh->mNumVertices;
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

	auto renderable = actor->AddComponent<Renderable>();
	//TODO : BoundBox

	model->AddMesh(vertices, indices, renderable);

	if (assimp_scene->HasMaterials())
	{
		const auto assimp_material = assimp_scene->mMaterials[assimp_mesh->mMaterialIndex];
		model->AddMaterial(LoadMaterial(assimp_scene, assimp_material, model), renderable);
	}
}

auto ModelImporter::LoadMaterial(const aiScene * assimp_scene, aiMaterial * assimp_material, Model * model) -> std::shared_ptr<Material>
{
	if (!model || !assimp_material)
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

	const auto load_texture = [&model, &assimp_scene, &assimp_material, &material, this](const aiTextureType& assimp_type, const TextureType& native_type)
	{
	    aiString texture_path;
		if (assimp_material->GetTextureCount(assimp_type) > 0)
		{
			if (AI_SUCCESS == assimp_material->GetTexture(assimp_type, 0, &texture_path))
			{
				//const auto deduced_path = AssimpHelper::ValidatePath(texture_path.data, model_path);
				//if(FileSystem::IsSupportedTextureFile(deduced_path))
				//  model->AddTexture(material, native_type, deduced_path);

				////embedded_texture : �𵨿� ���Ե� �ؽ�ó(�̹���) ������ �ִ��� Ȯ��
				//else if (const auto embedded_texture = assimp_scene->GetEmbeddedTexture(FileSystem::GetFileNameFromPath(texture_path.data).c_str()))
				//{
				//    const auto path = Texture2D::SaveEmbeddedTextureToFile
				//	(
				//	    model->GetTextureDirectory() + FileSystem::GetFileNameFromPath(embedded_texture->mFilename.data),
				//	    embedded_texture->achFormatHint,
				//		embedded_texture->mWidth,
				//		embedded_texture->mHeight,
				//		embedded_texture->pcData
				//	);

				//	if(path!=NOT_ASSIGNED_STR)
				//	  model->AddTexture(material, native_type, path);
				//}

				if(assimp_type == aiTextureType_DIFFUSE)
				   material->SetAlbedoColor(Color4::White);

                
				if (native_type == TextureType::Normal || native_type == TextureType::Height)
				{
					if (const auto texture = material->GetTexture(native_type))
					{
					   auto property_type = native_type;
					   //Height
					   property_type=(property_type==TextureType::Normal&&texture->IsGrayscale()) ? TextureType::Height : property_type;
					   //Normal
					   property_type = (property_type == TextureType::Height&&!texture->IsGrayscale()) ? TextureType::Normal : property_type;

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
	load_texture(aiTextureType_SHININESS, TextureType::Roughness);
	//AMBIENT : �ڽ��� ������ �ִ� ���� ��
	load_texture(aiTextureType_AMBIENT, TextureType::Metallic);
	load_texture(aiTextureType_NORMALS, TextureType::Normal);
	//Occlusion: ���⼺�� ������ �ʴ� �׸���
	load_texture(aiTextureType_LIGHTMAP, TextureType::Occlusion);
	load_texture(aiTextureType_EMISSIVE, TextureType::Emissive);
	load_texture(aiTextureType_HEIGHT, TextureType::Height);
	load_texture(aiTextureType_OPACITY, TextureType::Mask);

	return material;
}
