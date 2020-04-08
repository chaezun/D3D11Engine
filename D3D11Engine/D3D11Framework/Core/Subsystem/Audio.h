#pragma once
#include "ISubsystem.h"

namespace FMOD { class System; }

class Audio final : public ISubsystem
{
public:
	Audio(class Context* context);
	~Audio();

	const bool Initialize() override;

	auto GetFMODSystem() const -> FMOD::System* { return system; }

	void SetListenerTransform(class Transform* transform);

	void Update();

private:
	void LogErrorHandler(const int& error) const;

private:
	FMOD::System* system = nullptr;
	class Transform* listener = nullptr;
	float distance_factor = 1.0f;
	uint max_channels = 32;
	int result = 0;
	bool is_initialized = false;
};