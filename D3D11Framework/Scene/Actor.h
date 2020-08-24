#pragma once
#include "Component/IComponent.h"

//enable_shared_from_this : shared_ptr의 성질을 상속시킴 row->shared로 변환하기 위해 필요
class Actor final : public D3D11_Object , public std::enable_shared_from_this<Actor>
{
public:
	Actor(class Context* context);
	~Actor();

	void Initialize(const std::shared_ptr<class Transform>& transform);

	void Start();
	void Update();
	void Stop();

	//=================================================================================================
	// [Property]
	//=================================================================================================
	auto GetName() const ->const std::string& { return name; }
	auto SetName(const std::string& name) { this->name = name; }

	auto IsActive() const ->const bool& { return is_active; }
	auto SetActive(const bool& is_active) { this->is_active = is_active; }

	auto GetTransform() const -> const std::shared_ptr<class Transform>& { return transform; }
	auto GetTransform_Raw() const -> class Transform* { return transform.get(); }

	auto GetRenderable() const -> const std::shared_ptr<class Renderable>& { return renderable; }
	auto GetRenderable_Raw() const -> class Renderable* { return renderable.get(); }

	//=================================================================================================
	// [Component]
	//=================================================================================================
	auto AddComponent(const ComponentType& type)->std::shared_ptr<IComponent>;

	template <typename T>
	auto AddComponent() -> std::shared_ptr<T>;

	template <typename T>
	auto GetComponent() -> std::shared_ptr<T>;

	template <typename T>
	auto GetComponent_Raw() -> T*;

	template <typename T>
	auto GetComponents() -> std::vector<std::shared_ptr<T>>;

	auto GetAllComponents() const ->const std::vector<std::shared_ptr<IComponent>>& {return components;}

	auto HasComponent(const ComponentType& type) -> const bool;

	template <typename T>
	auto HasComponent() -> const bool;

	template <typename T>
	void RemoveComponent();

private:
	class Context* context = nullptr;
	std::string name = NOT_ASSIGNED_STR;
	bool is_active = true;

	std::shared_ptr<class Transform> transform;
	std::shared_ptr<class Renderable> renderable;
	std::vector<std::shared_ptr<IComponent>> components;
};

template<typename T>
inline auto Actor::AddComponent() -> std::shared_ptr<T>
{
    static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");
	
	auto type=IComponent::DeduceComponentType<T>();

	if(HasComponent(type) && type != ComponentType::Script)
	  return GetComponent<T>();

    components.emplace_back
	(
		std::make_shared<T>
		(
		   context,
		   this,
		   transform.get()
		)
	);

	auto new_component=std::static_pointer_cast<T>(components.back());

	new_component->SetComponentType(type);

	//if (type == ComponentType::Renderable)
	//{
	//	renderable=new_component;
	//}

	//if constexpr: 뒤의 else 문을 판별하지 않고 넘어감
	if constexpr (std::is_same<T, Renderable>::value)
	{
		renderable = new_component;
	}

	WORK_EVENT(EventType::Scene_Refresh);

	return new_component;
}

template<typename T>
inline auto Actor::GetComponent() -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	for (const auto& component : components)
	{
		if(component->GetComponentType()==type)
		   return std::static_pointer_cast<T>(component);
	}

	return nullptr;
}

template<typename T>
inline auto Actor::GetComponent_Raw() -> T *
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	for (const auto& component : components)
	{
		if (component->GetComponentType() == type)
			return std::static_pointer_cast<T>(component).get();
	}

	return nullptr;
}

template<typename T>
inline auto Actor::GetComponents() -> std::vector<std::shared_ptr<T>>
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	std::vector<std::shared_ptr<T>> temp_components;

	for (const auto& component : components)
	{
		if (component->GetComponentType() != type)
		   continue;

		temp_components.emplace_back(std::static_pointer_cast<T>(component));
	}

	return temp_components;
}

template<typename T>
inline auto Actor::HasComponent() -> const bool
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");
	
	return HasComponent(IComponent::DeduceComponentType<T>());
}

template<typename T>
inline void Actor::RemoveComponent()
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	for (auto iter = components.begin(); iter != components.end();)
	{
		auto component=*iter;
		if (component->GetComponentType() == type)
		{
			component.reset();
			iter=components.erase(iter);
		}

		else 
		  iter++;
	}
}
