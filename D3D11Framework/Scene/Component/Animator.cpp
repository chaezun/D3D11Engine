#include "D3D11Framework.h"
#include "Animator.h"

Animator::Animator(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
	ZeroMemory(&cpu_buffer, sizeof(ANIMATION_DATA));

	skinned_transforms.reserve(MAX_BONE_COUNT);
	skinned_transforms.resize(MAX_BONE_COUNT);

}

Animator::~Animator()
{
}

void Animator::OnStart()
{
}

void Animator::OnUpdate()
{
	if (Engine::IsFlagEnabled(EngineFlags_Game))
	{

		if (current_animation.expired() || animation_mode != AnimationMode::Play)
			return;

		auto timer = context->GetSubsystem<Timer>();

		float ticks_per_second = current_animation.lock()->GetTicksPerSecond();
		float time_in_ticks = timer->GetDeltaTimeSec() * ticks_per_second;
		float duration = current_animation.lock()->GetDuration();
		float animation_time = fmod(time_in_ticks, duration);

		if (!current_animation.lock()->GetAnimationEnd())
		{
			animation_time_elapsed += animation_time;
		}
		else
		{
			current_animation.lock()->SetAnimationEnd(false);
			animation_time_elapsed = 0.0f;
		}

		std::vector<Matrix> animation_transforms(skeleton->GetBoneCount());
		current_animation.lock()->CalcAnimationTransforms(animation_transforms, animation_time_elapsed);

		for (uint i = 0; i < animation_transforms.size(); i++)
		{
			auto bone_offset = skeleton->GetBoneOffset(i);
			skinned_transforms[i] = bone_offset * animation_transforms[i];
		}
	}
}

void Animator::OnStop()
{
}

void Animator::Play()
{
}

void Animator::Pause()
{
}

void Animator::Stop()
{
}

void Animator::SetCurrentAnimation(const std::string & name)
{
	if (animations.find(name) == animations.end())
	{
		LOG_ERROR("Animation not found");
		return;
	}

	current_animation = animations[name];
}

void Animator::SetAnimations(const std::map<std::string, std::shared_ptr<Animation>>& animations)
{
	this->animations.clear();
	this->animations = animations;
}

auto Animator::GetAnimation(const std::string & name) -> std::shared_ptr<Animation>
{
	if (animations.find(name) == animations.end())
	{
		LOG_ERROR("Animation not found");
		return nullptr;
	}

	return animations[name];
}

void Animator::AddAnimation(const std::string & name, const std::shared_ptr<Animation>& animation)
{
	if (animations.find(name) != animations.end())
	{
		LOG_ERROR("The animation already exist");
		return;
	}

	//TODO : bone matching

	animations[name] = animation;
}

void Animator::UpdateConstantBuffer()
{
	if (!gpu_buffer)
	{
		gpu_buffer = std::make_shared<ConstantBuffer>(context);
		gpu_buffer->Create<ANIMATION_DATA>();
	}

	auto is_update = false;
	for (uint i = 0; i < skinned_transforms.size(); i++)
		is_update |= skinned_transforms[i] != cpu_buffer.skinned_transforms[i];

	if (!is_update)
		return;

	auto gpu_data = gpu_buffer->Map<ANIMATION_DATA>();
	if (!gpu_data)
	{
		LOG_ERROR("Failed to map buffe");
		return;
	}

	std::copy(skinned_transforms.begin(), skinned_transforms.end(), gpu_data->skinned_transforms);
	std::copy(skinned_transforms.begin(), skinned_transforms.end(), cpu_buffer.skinned_transforms);

	gpu_buffer->Unmap();
}