#include "D3D11Framework.h"
#include "Engine.h"
#include "Core/D3D11Graphics/CommandList.h"

uint Engine::engine_flags = 0U;

Engine::Engine()
{
    engine_flags=
	    EngineFlags_Update|
		EngineFlags_Render;

	context = new Context();
	context->RegisterSubsystem<Timer>();
	context->RegisterSubsystem<Input>();
	context->RegisterSubsystem<Graphics>();
	context->RegisterSubsystem<Thread>();
	context->RegisterSubsystem<Scripting>();
	context->RegisterSubsystem<ResourceManager>();
	context->RegisterSubsystem<SceneManager>();
	context->RegisterSubsystem<Renderer>();

	context->Initialize_Subsystems();
}

Engine::~Engine()
{
	SAFE_DELETE(context);
}

void Engine::Update()
{
	WORK_EVENT(EventType::Frame_Start);

	if (IsFlagEnabled(EngineFlags_Update))
		WORK_EVENT(EventType::Update);

	if (IsFlagEnabled(EngineFlags_Render))
		WORK_EVENT(EventType::Render);

	WORK_EVENT(EventType::Frame_End);
}
