#pragma once
#include "IComponent.h"

class AudioSource final : public IComponent
{
public:
	AudioSource
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~AudioSource();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetAudioClipName() const -> const std::string;
	void SetAudioClip(const std::string& path);

	auto Play() -> const bool;
	auto Pause() -> const bool;
	auto Stop() -> const bool;

	auto IsLoop() const -> const bool& { return is_loop; }
	void SetLoop(const bool& is_loop);

	auto IsMute() const -> const bool& { return is_mute; }
	void SetMute(const bool& is_mute);

	auto GetPriority() const -> const int& { return priority; }
	void SetPriority(const int& priority);

	auto GetVolume() const -> const float& { return volume; }
	void SetVolume(const float& volume);

	auto GetPitch() const -> const float& { return pitch; }
	void SetPitch(const float& pitch);

	auto GetPan() const -> const float& { return pan; }
	void SetPan(const float& pan);

	auto IsPlaying() const -> const bool& { return is_playing; }
	void SetPlaying(const bool& is_playing) { this->is_playing = is_playing; }

private:
	std::shared_ptr<class AudioClip> audio_clip;
	bool is_loop = false;
	bool is_mute = false;
	int priority = 128;
	float volume = 1.0f;
	float pitch = 1.0f;
	float pan = 0.0f;
	bool is_playing = false;
};