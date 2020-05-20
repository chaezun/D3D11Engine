#pragma once
#include "IResource.h"

#define MAX_VERTEX_WEIGHTS 4
struct AnimationVertexWeights final
{
	AnimationVertexWeights()
	{
		ZeroMemory(bone_ids, sizeof(uint) * MAX_VERTEX_WEIGHTS);
		ZeroMemory(weights, sizeof(float) * MAX_VERTEX_WEIGHTS);
	}

	void AddData(const uint& bone_id, const float& weight)
	{
		for (uint i = 0; i < MAX_VERTEX_WEIGHTS; i++)
		{
			if (weights[i] == 0.0f)
			{
				bone_ids[i] = bone_id;
				weights[i] = weight;
				return;
			}
		}
	}

	uint bone_ids[MAX_VERTEX_WEIGHTS];
	float weights[MAX_VERTEX_WEIGHTS];
};

struct KeyVector final
{
	double time;
	Vector3 value;
};

struct KeyQuaternion final
{
	double time;
	Quaternion value;
};

struct AnimationNode
{
	std::string name;
	std::vector<KeyVector> scale_frames;
	std::vector<KeyVector> position_frames;
	std::vector<KeyQuaternion> rotation_frames;
};

class Animation final : public IResource
{
public:
	Animation(class Context* context);
	~Animation();

	const bool SaveToFile(const std::string& path) override;
	const bool LoadFromFile(const std::string& path) override;

	auto GetName() const -> const std::string& { return name; }
	void SetName(const std::string& name) { this->name = name; }

	auto GetDuration() const -> const float& { return static_cast<float>(duration); }
	void SetDuration(const double& duration) { this->duration = duration; }

	auto GetTicksPerSecond() const -> const float& { return static_cast<float>(ticks_per_second); }
	void SetTicksPerSecond(const double& ticks_per_second) { this->ticks_per_second = ticks_per_second; }

	auto GetSkeleton() const -> const std::shared_ptr<Skeleton>& { return skeleton; }
	void SetSkeleton(const std::shared_ptr<Skeleton>& skeleton) { this->skeleton = skeleton; }

	void AddAnimationNode(const AnimationNode& animation_node);

	void CalcAnimationTransforms(std::vector<Matrix>& animation_transforms, const float& time);

	void SortAnimationNodes(const std::shared_ptr<Skeleton>& skeleton);

private:
	auto CalcInterpolatedScaling(const uint& animation_node_index, const float& time) -> const Vector3;
	auto CalcInterpolatedPosition(const uint& animation_node_index, const float& time) -> const Vector3;
	auto CalcInterpolatedRotation(const uint& animation_node_index, const float& time) -> const Quaternion;

	auto FindScaling(const uint& animation_node_index, const float& time) -> const int;
	auto FindPosition(const uint& animation_node_index, const float& time) -> const int;
	auto FindRotation(const uint& animation_node_index, const float& time) -> const int;

private:
	std::string name = "";
	double duration = 0.0;
	double ticks_per_second = 0.0;

	std::shared_ptr<Skeleton> skeleton;
	std::vector<AnimationNode> animation_nodes;

    uint animation_node_index = 0;
};