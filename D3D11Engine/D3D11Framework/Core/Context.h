#pragma once
#include "D3D11Framework.h"
#include "Subsystem/ISubsystem.h"

class Context final
{
public:
	Context() = default;
	~Context()
	{
		//for (int i = static_cast<int>(subsystems.size() - 1); i >= 0; i--)
		//    SAFE_DELETE(subsystems[i]);

		for (auto iter = subsystems.rbegin(); iter != subsystems.rend(); iter++)
			SAFE_DELETE(*iter);
	}

	auto Initialize_Subsystems() -> const bool
	{
		for (const auto& subsystem : subsystems)
		{
			if (!subsystem->Initialize())
			{
				assert(false);
				return false;
			}
		}

		return true;
	}

	template <typename T>
	auto RegisterSubsystem()->T*;

	template <typename T>
	auto GetSubsystem()->T*;

private:
	std::vector<ISubsystem*> subsystems;
};

template<typename T>
inline auto Context::RegisterSubsystem() -> T *
{
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	return static_cast<T*>(subsystems.emplace_back(new T(this)));
}

template<typename T>
inline auto Context::GetSubsystem() -> T *
{
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	for (const auto& subsystem : subsystems)
	{
		if (typeid(T) == typeid(*subsystem))
			return static_cast<T*>(subsystem);
	}
	return nullptr;
}
