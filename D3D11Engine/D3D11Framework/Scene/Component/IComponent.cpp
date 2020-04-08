#include "D3D11Framework.h"
#include "IComponent.h"
#include "Transform.h"
#include "Renderable.h"
#include "Camera.h"
#include "Scene/Actor.h"
#include "Light.h"
#include "SkyBox.h"
#include "Script.h"
#include "AudioSource.h"
#include "AudioListener.h"

IComponent::IComponent(Context * context, Actor * actor, Transform * transform)
: context(context),
  actor(actor),
  transform(transform)
{

}

auto IComponent::GetActorName() const -> const std::string
{
	if(!actor)
	   return "";
	
	return actor->GetName();
}

template<typename T>
constexpr ComponentType IComponent::DeduceComponentType()
{
	return ComponentType::Unknown;
}

#define REGISTER_COMPONENT_TYPE(T, enum_type) template <> ComponentType IComponent::DeduceComponentType<T>() {return enum_type;}

REGISTER_COMPONENT_TYPE(Transform, ComponentType::Transform);
REGISTER_COMPONENT_TYPE(Renderable, ComponentType::Renderable);
REGISTER_COMPONENT_TYPE(Camera, ComponentType::Camera);
REGISTER_COMPONENT_TYPE(Light, ComponentType::Light);
REGISTER_COMPONENT_TYPE(SkyBox, ComponentType::SkyBox);
REGISTER_COMPONENT_TYPE(Script, ComponentType::Script);
REGISTER_COMPONENT_TYPE(AudioSource, ComponentType::AudioSource)
REGISTER_COMPONENT_TYPE(AudioListener, ComponentType::AudioListener)
