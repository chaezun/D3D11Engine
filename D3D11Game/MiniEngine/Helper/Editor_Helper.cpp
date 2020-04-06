#include "stdafx.h"
#include "Editor_Helper.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Terrain.h"

void Editor_Helper::Initialize(Context * context)
{
	this->context          = context;
	this->resource_manager = context->GetSubsystem<ResourceManager>();
	this->scene_manager    = context->GetSubsystem<SceneManager>();
	this->thread           = context->GetSubsystem<Thread>();
	this->renderer         = context->GetSubsystem<Renderer>();
	this->input            = context->GetSubsystem<Input>();
}

void Editor_Helper::LoadModel(const std::string & path)
{
  thread->AddTask([this, path]() 
  {
      resource_manager->Load<Model>(path);
  });
}

void Editor_Helper::Picking()
{
    const auto& camera = renderer->GetCamera();
	if(!camera)
	  return;

    std::shared_ptr<Actor> actor;
	camera->Pick(input->GetMousePosition(), actor);

	if (actor)
	{
		if(auto terrain = actor->GetComponent<Terrain>())
		   terrain->SetTerrainSelected(true);
	}

	selected_actor = actor;
}
