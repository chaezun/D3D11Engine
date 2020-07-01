#include "D3D11Framework.h"
#include "AudioSource.h"

AudioSource::AudioSource(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
}

AudioSource::~AudioSource()
{
}

void AudioSource::OnStart()
{
	if (!is_playing)
		return;

	Play();
}

void AudioSource::OnUpdate()
{
	if (!audio_clip)
		return;

	audio_clip->Update();
}

void AudioSource::OnStop()
{
	Stop();
}

auto AudioSource::GetAudioClipName() const -> const std::string
{
	return audio_clip ? audio_clip->GetResourceName() : NOT_ASSIGNED_STR;
}

void AudioSource::SetAudioClip(const std::string & path)
{
	auto resource_manager = context->GetSubsystem<ResourceManager>();
	audio_clip = resource_manager->Load<AudioClip>(path);

	if (audio_clip)
		audio_clip->SetTransform(transform);
}

auto AudioSource::Play() -> const bool
{
	if (!audio_clip)
		return false;

	audio_clip->Play();
	audio_clip->SetLoop(is_loop);
	audio_clip->SetMute(is_mute);
	audio_clip->SetPriority(priority);
	audio_clip->SetVolume(volume);
	audio_clip->SetPitch(pitch);
	audio_clip->SetPan(pan);

	return true;
}

auto AudioSource::Pause() -> const bool
{
	if (!audio_clip)
		return false;

	return audio_clip->Pause();
}

auto AudioSource::Stop() -> const bool
{
	if (!audio_clip)
		return false;

	return audio_clip->Stop();
}

void AudioSource::SetLoop(const bool & is_loop)
{
	if (!audio_clip || this->is_loop == is_loop)
		return;

	this->is_loop = is_loop;
	audio_clip->SetLoop(this->is_loop);
}

void AudioSource::SetMute(const bool & is_mute)
{
	if (!audio_clip || this->is_mute == is_mute)
		return;

	this->is_mute = is_mute;
	audio_clip->SetMute(this->is_mute);
}

void AudioSource::SetPriority(const int & priority)
{
	if (!audio_clip || this->priority == priority)
		return;

	//Channel Priority
	//0     : 우선순위가 높음
	//255   : 우선순위가 낮음
	//128   : 기본값

	this->priority = Math::Clamp(priority, 0, 255);
	audio_clip->SetPriority(this->priority);
}

void AudioSource::SetVolume(const float & volume)
{
	if (!audio_clip || this->volume == volume)
		return;

	this->volume = Math::Clamp(volume, 0.0f, 1.0f);
	audio_clip->SetVolume(this->volume);
}

void AudioSource::SetPitch(const float & pitch)
{
	if (!audio_clip || this->pitch == pitch)
		return;

	this->pitch = Math::Clamp(pitch, 0.0f, 3.0f);
	audio_clip->SetPitch(this->pitch);
}

void AudioSource::SetPan(const float & pan)
{
	if (!audio_clip || this->pan == pan)
		return;

	this->pan = Math::Clamp(pan, -1.0f, 1.0f);
	audio_clip->SetPan(this->pan);
}
