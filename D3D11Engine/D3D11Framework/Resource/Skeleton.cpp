#include "D3D11Framework.h"
#include "Skeleton.h"

Skeleton::Skeleton(Context * context)
	: IResource(context)
{
}

Skeleton::~Skeleton()
{
	bones.clear();
	bones.shrink_to_fit();
}

const bool Skeleton::SaveToFile(const std::string & path)
{
	return false;
}

const bool Skeleton::LoadFromFile(const std::string & path)
{
	return false;
}

void Skeleton::AddBone(const Bone & bone)
{
	bones.emplace_back(bone);
}

void Skeleton::AddBone(const std::string & name, const int & parent_index, const Matrix & offset)
{
	bones.emplace_back(name, parent_index, offset);
}

auto Skeleton::FindBoneIndex(const std::string & name) -> const int
{
	if (bones.empty())
	{
		LOG_ERROR("Bones is empty");
		return -1;
	}

	for (uint i = 0; i < bones.size(); i++)
	{
		if (bones[i].name == name)
			return i;
	}

	return -1;
}

auto Skeleton::FindBone(const std::string & name) -> Bone *
{
	if (bones.empty())
	{
		LOG_ERROR("Bones is empty");
		return nullptr;
	}

	for (auto& bone : bones)
	{
		if (bone.name == name)
			return &bone;
	}

	return nullptr;
}

auto Skeleton::FindBone(const uint & bone_index) -> Bone *
{
	if (bones.empty())
	{
		LOG_ERROR("Bones is empty");
		return nullptr;
	}

	if (bone_index >= bones.size())
	{
		LOG_ERROR("Invaild parameter, Out of range");
		return nullptr;
	}

	return &bones[bone_index];
}
