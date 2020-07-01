#include "D3D11Framework.h"
#include "AudioListener.h"

AudioListener::AudioListener(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
	audio = context->GetSubsystem<Audio>();
}

AudioListener::~AudioListener()
{
	audio = nullptr;
}

void AudioListener::OnStart()
{
}

void AudioListener::OnUpdate()
{
	if (!audio)
		return;

	audio->SetListenerTransform(transform);
}

void AudioListener::OnStop()
{
}
