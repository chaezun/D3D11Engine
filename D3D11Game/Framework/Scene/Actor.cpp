#include "Framework.h"
#include "Actor.h"
#include "Scene.h"
#include "Component/Transform.h"

Actor::Actor(Context * context)
:context(context)
{

}

Actor::~Actor()
{
   components.clear();
   components.shrink_to_fit();
}

void Actor::Initialize(const std::shared_ptr<class Transform>& transform)
{
    this->transform=transform;
}

void Actor::Start()
{
    if(!is_active)
	  return;

	for (const auto& component : components)
	{
		if (!component->IsEnabled())
		{
		   continue;
		}

		component->OnStart();
	}
}

void Actor::Update()
{
	if (!is_active)
		return;

	for (const auto& component : components)
	{
		if (!component->IsEnabled())
		{
			continue;
		}

		component->OnUpdate();
	}
}

void Actor::Stop()
{
	if (!is_active)
		return;

	for (const auto& component : components)
	{
		if (!component->IsEnabled())
		{
			continue;
		}

		component->OnStop();
	}
}

auto Actor::AddComponent(const ComponentType & type) -> std::shared_ptr<IComponent>
{
	std::shared_ptr<IComponent> component;

	switch (type)
	{
	case ComponentType::Unknown:
		break;
	case ComponentType::Camera:
		break;
	case ComponentType::SkyBox:
		break;
	case ComponentType::Transform:
		break;
	case ComponentType::Renderable:
		break;
	case ComponentType::Script:
		break;
	case ComponentType::Terrain:
		break;
	case ComponentType::Light:
		break;
	case ComponentType::Collider:
		break;
	case ComponentType::Audto:
		break;
	case ComponentType::RigidBody:
		break;
	case ComponentType::Constraint:
		break;
	
	}
	
	return std::shared_ptr<IComponent>();
}

auto Actor::HasComponent(const ComponentType & type) -> const bool
{
	for (const auto& component : components)
	{
		if(component->GetComponentType()==type)
		 return true;
	}
	return false;
}
