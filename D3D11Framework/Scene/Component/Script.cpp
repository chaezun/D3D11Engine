#include "D3D11Framework.h"
#include "Script.h"
#include "Script/Script_Instance.h"

Script::Script(Context * context, Actor * actor, Transform * transform)
	:IComponent(context, actor, transform)
{
}

void Script::OnStart()
{
	if (!script_instance)
		return;

	if (!script_instance->IsInstantiated())
		return;

	script_instance->ExecuteStart();
	is_stopped = false;
}

void Script::OnUpdate()
{
	if (is_stopped)
		return;

	if (!script_instance)
		return;

	if (!script_instance->IsInstantiated())
		return;

	script_instance->ExecuteUpdate();
}

void Script::OnStop()
{
	is_stopped = true;
}

auto Script::GetScriptPath() -> const std::string
{
	return script_instance ? script_instance->GetScriptPath() : NOT_ASSIGNED_STR;
}

auto Script::GetScriptName() -> const std::string
{
	return script_instance ? FileSystem::GetIntactFileNameFromPath(GetScriptPath()) : NOT_ASSIGNED_STR;
}

auto Script::SetScript(const std::string & path) -> const bool
{
	script_instance = std::make_shared<Script_Instance>();
	return script_instance->Instantiate(context->GetSubsystem<Scripting>(), actor, path);
}
