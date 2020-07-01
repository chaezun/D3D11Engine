#include "D3D11Framework.h"
#include "AudioClip.h"
#include "Scene/Component/Transform.h"

#pragma warning(push, 0)
#include <fmod.hpp>
#include <fmod_errors.h>
#pragma warning(pop)

AudioClip::AudioClip(Context * context)
	: IResource(context)
{
	loop_mode = FMOD_LOOP_OFF;
	rolloff_mode = FMOD_3D_LINEARROLLOFF;

	system = context->GetSubsystem<Audio>()->GetFMODSystem();
}

AudioClip::~AudioClip()
{
	if (!sound)
		return;

	result = sound->release();
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return;
	}
}

const bool AudioClip::SaveToFile(const std::string & path)
{
	return false;
}

const bool AudioClip::LoadFromFile(const std::string & path)
{
	sound = nullptr;
	channel = nullptr;

	play_mode == PlayMode::Memory ? CreateSound(path) : CreateStream(path);

	return true;
}

auto AudioClip::Play() -> const bool
{
	if (IsChannelValid())
	{
		if (IsPlaying())
			return true;
	}

	result = system->playSound(sound, nullptr, false, &channel);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::Pause() -> const bool
{
	if (!IsChannelValid())
		return true;

	auto is_paused = false;
	result = channel->getPaused(&is_paused);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	if (is_paused)
		return true;

	result = channel->setPaused(true);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::Stop() -> const bool
{
	if (!IsChannelValid())
		return true;

	if (!IsPlaying())
		return true;

	result = channel->stop();
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	channel = nullptr;

	return true;
}

auto AudioClip::SetLoop(const bool & is_loop) -> const bool
{
	loop_mode = is_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

	if (!sound)
		return false;

	if (is_loop)
	{
		result = sound->setLoopCount(-1);
		if (result != FMOD_OK)
		{
			LogErrorHandler(result);
			return false;
		}
	}

	result = sound->setMode(GetSoundMode());
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetMute(const bool & is_mute) -> const bool
{
	if (!IsChannelValid())
		return false;

	result = channel->setMute(is_mute);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetPriority(const int & priority) -> const bool
{
	if (!IsChannelValid())
		return false;

	result = channel->setPriority(priority);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetVolume(const float & volume) -> const bool
{
	if (!IsChannelValid())
		return false;

	result = channel->setVolume(volume);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	result = channel->set3DMinMaxDistance(min_distance, max_distance);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetPitch(const float & pitch) -> const bool
{
	if (!IsChannelValid())
		return false;

	result = channel->setPitch(pitch);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetPan(const float & pan) -> const bool
{
	if (!IsChannelValid())
		return false;

	result = channel->setPan(pan);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetRolloff(const std::vector<Vector3>& curve_points) -> const bool 
{
	if (!IsChannelValid())
		return false;

	SetRolloff(Rolloff::Custom);

	std::vector<FMOD_VECTOR> fmod_curves;
	for (const auto& point : curve_points)
		fmod_curves.emplace_back(FMOD_VECTOR{ point.x, point.y, point.z }); //Uniform Initialization

	result = channel->set3DCustomRolloff(fmod_curves.data(), static_cast<int>(fmod_curves.size()));
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::SetRolloff(const Rolloff & rolloff) -> const bool
{
	return rolloff_mode = rolloff == Rolloff::Linear ? FMOD_3D_LINEARROLLOFF : FMOD_3D_CUSTOMROLLOFF;
}

auto AudioClip::IsPlaying() -> const bool
{
	if (!IsChannelValid())
		return false;

	auto is_playing = false;
	result = channel->isPlaying(&is_playing);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return is_playing;
}

void AudioClip::Update()
{
	if (!IsChannelValid() || !transform)
		return;

	const auto position = transform->GetTranslation();

	FMOD_VECTOR fmod_position = { position.x, position.y, position.z };
	FMOD_VECTOR fmod_velocity = { 0.0f, 0.0f, 0.0f };

	result = channel->set3DAttributes(&fmod_position, &fmod_velocity);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return;
	}
}

auto AudioClip::CreateSound(const std::string & path) -> const bool
{
	SetRolloff(Rolloff::Linear);

	result = system->createSound(path.c_str(), GetSoundMode(), nullptr, &sound);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	result = sound->set3DMinMaxDistance(min_distance, max_distance);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::CreateStream(const std::string & path) -> const bool
{
	SetRolloff(Rolloff::Linear);

	result = system->createStream(path.c_str(), GetSoundMode(), nullptr, &sound);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	result = sound->set3DMinMaxDistance(min_distance, max_distance);
	if (result != FMOD_OK)
	{
		LogErrorHandler(result);
		return false;
	}

	return true;
}

auto AudioClip::GetSoundMode() const -> const int
{
	return FMOD_3D | loop_mode | rolloff_mode;
}

auto AudioClip::IsChannelValid() const -> const bool
{
	if (!channel)
		return false;

	bool dummy = false;
	return channel->isPlaying(&dummy) == FMOD_OK;
}

void AudioClip::LogErrorHandler(const int & error) const
{
	LOG_ERROR(std::string(FMOD_ErrorString(static_cast<FMOD_RESULT>(error))));
}
