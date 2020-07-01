#pragma once
#include "IComponent.h"

class Script final : public IComponent
{
public:
	Script
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Script() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetScriptPath() -> const std::string;
	auto GetScriptName() -> const std::string;

	auto SetScript(const std::string& path) -> const bool;

private:
    std::shared_ptr<class Script_Instance> script_instance;
	bool is_stopped = true;
};