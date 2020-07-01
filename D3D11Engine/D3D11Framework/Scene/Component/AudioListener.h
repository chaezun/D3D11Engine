#pragma once
#include "IComponent.h"

class AudioListener final : public IComponent
{
public:
	AudioListener
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~AudioListener();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

private:
	class Audio* audio = nullptr;
};