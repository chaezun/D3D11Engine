#pragma once
#include "IResource.h"

struct Bone final
{
	Bone() = default;
	Bone(const std::string& name, const int& parent_index, const Matrix& offset)
		: name(name)
		, parent_index(parent_index)
		, offset(offset)
	{}

	std::string name = "";
	int parent_index = -1;
	Matrix offset = Matrix::Identity;
};

class Skeleton final : public IResource
{
public:
	Skeleton(class Context* context);
	~Skeleton();

	const bool SaveToFile(const std::string& path) override;
	const bool LoadFromFile(const std::string& path) override;

	auto GetBones() const -> const std::vector<Bone>& { return bones; }
	void SetBones(const std::vector<Bone>& bones) { this->bones = bones; }

	void AddBone(const Bone& bone);
	void AddBone(const std::string& name, const int& parent_index, const Matrix& offset);

	auto FindBoneIndex(const std::string& name) -> const int;
	auto FindBone(const std::string& name)->Bone*;
	auto FindBone(const uint& bone_index)->Bone*;

	auto GetBoneCount() const { return static_cast<uint>(bones.size()); }
	auto GetBoneOffset(const uint& bone_index) { return bones[bone_index].offset; }
	auto GetBoneParentIndex(const uint& bone_index) { return bones[bone_index].parent_index; }

private:
	std::vector<Bone> bones;
};