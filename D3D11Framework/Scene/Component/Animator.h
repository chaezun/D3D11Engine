#pragma once
#include "IComponent.h"

enum class AnimationMode : uint
{
	Play, //Play 모드일 경우
	Pause,
	Stop, //Ediotr 모드일 경우
};

//ModelImporter에서 추출한 애니메이션 데이터를 ConstantBuffer에 연결해 적용시키는 클래스
class Animator final : public IComponent
{
public:
	Animator
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Animator();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	void Play();
	void Pause();
	void Stop();

	auto GetAnimationMode() const -> const AnimationMode& { return animation_mode; }
	void SetAnimationMode(const AnimationMode& mode) { this->animation_mode = mode; }

	auto GetSkeleton() const -> const std::shared_ptr<Skeleton>& { return skeleton; }
	void SetSkeleton(const std::shared_ptr<Skeleton>& skeleton) { this->skeleton = skeleton; }

	auto GetCurrentAnimation() -> const std::shared_ptr<Animation> { return current_animation.lock(); }
	void SetCurrentAnimation(const std::string& name);

	auto GetAnimations() const -> const std::map<std::string, std::shared_ptr<Animation>>& { return animations; }
	void SetAnimations(const std::map<std::string, std::shared_ptr<Animation>>& animations);

	auto GetAnimation(const std::string& name)->std::shared_ptr<Animation>;
	void AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation);
	auto GetAnimationCount() -> const int& { return animations.size(); }
	auto GetAnimationName(const int& index) -> const std::string;

	auto GetConstantBuffer() const -> const std::shared_ptr<ConstantBuffer>& { return gpu_buffer; }
	void UpdateConstantBuffer();

private:
	AnimationMode animation_mode = AnimationMode::Play;
	float frame_counter = 0.0f;

	std::weak_ptr<Animation> current_animation;
	std::map<std::string, std::shared_ptr<Animation>> animations;
	std::vector<std::string> animation_name;

	std::shared_ptr<Skeleton> skeleton;
	std::vector<Matrix> skinned_transforms;

	ANIMATION_DATA cpu_buffer;
	std::shared_ptr<ConstantBuffer> gpu_buffer;

	float animation_time_elapsed = 0.0f;
};