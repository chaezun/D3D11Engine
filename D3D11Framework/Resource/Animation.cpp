#include "D3D11Framework.h"
#include "Animation.h"

Animation::Animation(Context * context)
	: IResource(context, ResourceType::Animation)
{
}

Animation::~Animation()
{
}

const bool Animation::SaveToFile(const std::string & path)
{
	return false;
}

const bool Animation::LoadFromFile(const std::string & path)
{
	return false;
}

void Animation::AddAnimationNode(const AnimationNode & animation_node)
{
	animation_nodes.emplace_back(animation_node);
}

void Animation::CalcAnimationTransforms(std::vector<Matrix>& animation_transforms, const float & time)
{
	for (uint i = 0; i < animation_transforms.size(); i++)
	{
		auto scaling = CalcInterpolatedScaling(i, time);
		auto translation = CalcInterpolatedPosition(i, time);
		auto rotation = CalcInterpolatedRotation(i, time);
		auto animation_transform = Matrix(scaling, rotation, translation);

		auto parent_index = skeleton ? skeleton->GetBoneParentIndex(i) : -1;
		auto parent_transform = (parent_index != -1) ? animation_transforms[parent_index] : Matrix::Identity;
		
		animation_transforms[i] = animation_transform * parent_transform;
	}
}

void Animation::SortAnimationNodes(const std::shared_ptr<Skeleton>& skeleton)
{
	std::sort(animation_nodes.begin(), animation_nodes.end(), [&skeleton](const AnimationNode& lhs, const AnimationNode& rhs)
	{
		auto lhs_bone_index = skeleton->FindBoneIndex(lhs.name);
		auto rhs_bone_index = skeleton->FindBoneIndex(rhs.name);

		return lhs_bone_index < rhs_bone_index;
	});
}

auto Animation::CalcInterpolatedScaling(const uint & animation_node_index, const float & time) -> const Vector3
{
	if (animation_nodes.empty() || animation_nodes.size() <= animation_node_index || animation_nodes[animation_node_index].scale_frames.empty())
		return Vector3::One;

	auto scale_frames = animation_nodes[animation_node_index].scale_frames;

	if (scale_frames.size() == 1)
		return scale_frames.front().value;

	uint scaling_index = FindScaling(animation_node_index, time);
	uint next_scaling_index = scaling_index + 1;
	assert(next_scaling_index < scale_frames.size());

	float delta_time = static_cast<float>(scale_frames[next_scaling_index].time - scale_frames[scaling_index].time);
	float factor = (time - static_cast<float>(scale_frames[scaling_index].time)) / delta_time;

	if (!(factor >= 0.0f && factor <= 1.0f))
	{
		animation_end = true;
		return Vector3::One;
	}

	const auto& start = scale_frames[scaling_index].value;
	const auto& end = scale_frames[next_scaling_index].value;

	return Vector3::Lerp(start, end, factor);
}

auto Animation::CalcInterpolatedPosition(const uint & animation_node_index, const float & time) -> const Vector3
{
	if (animation_nodes.empty() || animation_nodes.size() <= animation_node_index || animation_nodes[animation_node_index].position_frames.empty())
		return Vector3::Zero;

	auto position_frames = animation_nodes[animation_node_index].position_frames;

	if (position_frames.size() == 1)
		return position_frames.front().value;

	uint position_index = FindPosition(animation_node_index, time);
	uint next_position_index = position_index + 1;
	assert(next_position_index < position_frames.size());

	float delta_time = static_cast<float>(position_frames[next_position_index].time - position_frames[position_index].time);
	float factor = (time - static_cast<float>(position_frames[position_index].time)) / delta_time;

	if (!(factor >= 0.0f && factor <= 1.0f))
	{
		animation_end = true;
		return Vector3::Zero;
	}

	const auto& start = position_frames[position_index].value;
	const auto& end = position_frames[next_position_index].value;

	return Vector3::Lerp(start, end, factor);
}

auto Animation::CalcInterpolatedRotation(const uint & animation_node_index, const float & time) -> const Quaternion
{
	if (animation_nodes.empty() || animation_nodes.size() <= animation_node_index || animation_nodes[animation_node_index].rotation_frames.empty())
		return Quaternion::Identity;

	auto rotation_frames = animation_nodes[animation_node_index].rotation_frames;

	if (rotation_frames.size() == 1)
		return rotation_frames.front().value;

	uint rotation_index = FindRotation(animation_node_index, time);
	uint next_rotation_index = rotation_index + 1;
	assert(next_rotation_index < rotation_frames.size());

	float delta_time = static_cast<float>(rotation_frames[next_rotation_index].time - rotation_frames[rotation_index].time);
	float factor = (time - static_cast<float>(rotation_frames[rotation_index].time)) / delta_time;

	if (!(factor >= 0.0f && factor <= 1.0f))
	{
		animation_end = true;
		return Quaternion::Identity;
	}

	const auto& start = rotation_frames[rotation_index].value;
	const auto& end = rotation_frames[next_rotation_index].value;

	return Quaternion::Slerp(start, end, factor);
}

auto Animation::FindScaling(const uint & animation_node_index, const float & time) -> const int
{
	if (animation_nodes.empty() || animation_nodes[animation_node_index].scale_frames.empty())
		return 0;

	auto& scale_frames = animation_nodes[animation_node_index].scale_frames;

	for (uint i = 0; i < scale_frames.size() - 1; i++)
	{
		if (time < static_cast<float>(scale_frames[i + 1].time))
			return i;
	}

	return 0;
}

auto Animation::FindPosition(const uint & animation_node_index, const float & time) -> const int
{
	if (animation_nodes.empty() || animation_nodes[animation_node_index].position_frames.empty())
		return 0;

	auto& position_frames = animation_nodes[animation_node_index].position_frames;

	for (uint i = 0; i < position_frames.size() - 1; i++)
	{
		if (time < static_cast<float>(position_frames[i + 1].time))
			return i;
	}

	return 0;
}

auto Animation::FindRotation(const uint & animation_node_index, const float & time) -> const int
{
	if (animation_nodes.empty() || animation_nodes[animation_node_index].rotation_frames.empty())
		return 0;

	auto& rotation_frames = animation_nodes[animation_node_index].rotation_frames;

	for (uint i = 0; i < rotation_frames.size() - 1; i++)
	{
		if (time < static_cast<float>(rotation_frames[i + 1].time))
			return i;
	}

	return 0;
}