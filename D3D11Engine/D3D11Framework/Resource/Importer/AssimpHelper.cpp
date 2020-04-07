#include "D3D11Framework.h"
#include "AssimpHelper.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

auto AssimpHelper::ToVector2(const aiVector2D & value) -> const Vector2
{
	return Vector2
	(
	   value.x,
	   value.y
	);
}

auto AssimpHelper::ToVector2(const aiVector3D & value) -> const Vector2
{
	return Vector2
	(
		value.x,
		value.y
	);
}

auto AssimpHelper::ToVector3(const aiVector3D & value) -> const Vector3
{
	return Vector3
	(
		value.x,
		value.y,
		value.z
	);
}

auto AssimpHelper::ToColor(const aiColor4D & value) -> const Color4
{
	return Color4
	(
		value.r,
		value.g,
		value.b,
		value.a
	);
}

auto AssimpHelper::ToQuaternion(const aiQuaternion & value) -> const Quaternion
{
	return Quaternion
	(
		value.x,
		value.y,
		value.z,
		value.w
	);
}

auto AssimpHelper::ToMatrix(const aiMatrix4x4 & value) -> const Matrix
{
	return Matrix
	(
	    value.a1, value.b1, value.c1, value.d1,
		value.a2, value.b2, value.c2, value.d2,
		value.a3, value.b3, value.c3, value.d3,
		value.a4, value.b4, value.c4, value.d4
	);
}

void AssimpHelper::ComputeNodeCount(aiNode * node, int * count)
{
   if(!node)
      return;

   *count++;

   for(uint i=0;i<node->mNumChildren; i++)
	   ComputeNodeCount(node->mChildren[i],count);
}

void AssimpHelper::ComputeActorTransform(aiNode * node, Actor * actor)
{
	if (!node||!actor)
	  return;

    auto matrix=ToMatrix(node->mTransformation);

	actor->GetTransform()->SetLocalScale(matrix.GetScale());
	actor->GetTransform()->SetLocalRotation(matrix.GetRotation());
	actor->GetTransform()->SetLocalTranslation(matrix.GetTranslation());
}

//같은 이름에 다른 확장자가 있는지 찾는 함수
auto AssimpHelper::Try_Multi_Extension(const std::string & path) -> const std::string
{
    const auto none_extension_file_path = FileSystem::GetPathWithoutExtension(path);

	auto supported_formats = FileSystem::GetSupportedTextureFormats();

	for (const auto& format : supported_formats)
	{
	   auto new_file_path=none_extension_file_path + format;
	   auto new_file_path_uppercase= none_extension_file_path + FileSystem::ToUppercase(format);

	   if(FileSystem::IsExistFile(new_file_path))
	      return new_file_path;

	   if (FileSystem::IsExistFile(new_file_path_uppercase))
		   return new_file_path_uppercase;
	}

	return path;
}

auto AssimpHelper::ValidatePath(const std::string & original_path, const std::string & model_path) -> const std::string
{
    const auto model_directory = FileSystem::GetDirectoryFromPath(model_path);
	auto full_texture_path =original_path;
	//original_path 1
	if (FileSystem::IsExistFile(full_texture_path))
	   return full_texture_path;

	//original_path 2
	full_texture_path = Try_Multi_Extension(full_texture_path);
	if (FileSystem::IsExistFile(full_texture_path))
		return full_texture_path;
    
	//model_directory + original_path 
	full_texture_path = model_directory + original_path;
	if (FileSystem::IsExistFile(full_texture_path))
		return full_texture_path;
 
    //1
    full_texture_path = Try_Multi_Extension(full_texture_path);
	if (FileSystem::IsExistFile(full_texture_path))
		return full_texture_path;
    
	//2
	full_texture_path = model_directory + FileSystem::GetFileNameFromPath(full_texture_path);
	if (FileSystem::IsExistFile(full_texture_path))
		return full_texture_path;

    //3
	full_texture_path = Try_Multi_Extension(full_texture_path);
	if (FileSystem::IsExistFile(full_texture_path))
		return full_texture_path;

	return NOT_ASSIGNED_STR;
}