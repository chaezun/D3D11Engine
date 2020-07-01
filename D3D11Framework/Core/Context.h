#pragma once
#include "D3D11Framework.h"
#include "Subsystem/ISubsystem.h"

//ISubsystem을 관리하는 클래스
class Context final
{
public:
	Context() = default;
	~Context()
	{
		//프로그래밍 원칙에서 먼저 할당한 데이터는 나중에 해제하는 것이 일반적이라
		//subsystems의 데이터 중 가장 나중에 들어온 데이터(rbegin)부터 차례대로 메모리 해제를 함
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
	auto RegisterSubsystem()->T*; //subsystem을 context에 등록하는 함수

	template <typename T> 
	auto GetSubsystem()->T*; //context에 등록한 subsystem을 반환하는 함수

private:
	std::vector<ISubsystem*> subsystems;
};

template<typename T>
inline auto Context::RegisterSubsystem() -> T *
{
	//std::is_base_of<class, T>::value : T가 class의 파생클래스(즉, 상속관계일 경우)일 경우 value는 true값을 가지고 아닐 경우에는 false를 가짐
	//static_assert : 왼쪽 인자의 값이 true일 경우 그냥 넘어가지만 false인 경우 오른쪽 메시지와 C2338 에러를 출력함  
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
