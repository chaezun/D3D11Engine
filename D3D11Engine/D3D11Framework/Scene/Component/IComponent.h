#pragma once

enum class ComponentType : uint
{
	Unknown,
	Camera,
	SkyBox,
	Transform,
	Renderable,
	Script,
	Terrain,
	Light,
	Collider,
	AudioSource,
	AudioListener,
	Animator,
	RigidBody,
	Constraint,
};

class IComponent :public D3D11_Object
{
public: 
   template <typename T>
   static constexpr ComponentType DeduceComponentType();

public:
	IComponent
	(
	class Context* context,
	class Actor* actor,
	class Transform* transform
	);
	
	virtual ~IComponent()=default;

	virtual void OnStart()=0;
	virtual void OnUpdate()=0;
	virtual void OnStop()=0;

	auto GetActor() const -> class Actor* {return actor;}
	auto GetActorName() const -> const std::string;
	auto GetTransform() const -> class Transform* {return transform;}

	auto GetComponentType() const -> const ComponentType& {return component_type;}
	auto SetComponentType(const ComponentType& type) {  this->component_type=type; }

	auto IsEnabled() const -> const bool& {return is_enabled;}
	void SetEnabled(const bool& is_enabled) {this->is_enabled=is_enabled;}

protected:
   class Context* context=nullptr;
   class Actor* actor=nullptr;
   class Transform* transform=nullptr;

   ComponentType component_type=ComponentType::Unknown;
   bool is_enabled=true;

};

