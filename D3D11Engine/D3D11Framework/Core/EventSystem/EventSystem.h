#pragma once
#include "D3D11Framework.h"

//EventSystem : Engine을 가동할때 각 클래스에서 언제 구동이 되는지 구분하기 위해 사용. -> 싱글톤으로 구현
enum class EventType : uint
{
	Frame_Start, //Engine이 처음 시작할 때
	Frame_End, //Engine이 종료될 때
	Update, //Engine이 계속 Update를 하는 경우
	Render, //Engine이 렌더링을 하는 경우
	Scene_Refresh, //Scene을 전환하는 경우
};

#define EVENT_HANDLER(function)      [this]() {function();} //해당 클래스의 특정함수 포인터를 반환(람다함수 사용)

#define WORK_EVENT(event_type)         EventSystem::Get().Do(event_type) //해당 EventType에 등록된 함수를 수행

#define SUBSCRIBE_TO_EVENT(event_type, function) EventSystem::Get().Subscribe(event_type, function) //Event등록
#define UNSUBSCRIBE_FROM_EVENT(event_type, function) EventSystem::Get().Unsubscribe(event_type, function) //Event등록 해제

class EventSystem final
{
public:
	typedef std::function<void()> Subscriber; //함수포인터를 사용하여 각 EventType때 수행할 함수들을 저장

public:
	static EventSystem& Get() //싱글톤으로 객체를 받아오기 위한 함수
	{
		static EventSystem instance;
		return instance;
	}

	void Subscribe(const EventType& event_type, Subscriber&& function)
	{
		subscriber_groups[event_type].emplace_back(std::forward<Subscriber>(function));
	}

	void Unsubscribe(const EventType& event_type, Subscriber&& function)
	{
		const size_t function_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&function));

		auto& subscriber_group = subscriber_groups[event_type];
		for (auto iter = subscriber_group.begin(); iter != subscriber_group.end();)
		{
			const size_t subscriber_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&(*iter)));
			if (subscriber_address == function_address)
			{
				iter = subscriber_group.erase(iter);
				return;
			}

			else
				iter++;
		}
	}

	void Do(const EventType& event_type)
	{
		if (subscriber_groups.find(event_type) == subscriber_groups.end()) //find() : 해당하는 EventType을 검색해서 못찾으면 end()를 반환
		{
			return;
		}

		for (const auto& subscriber : subscriber_groups[event_type]) //EventType에 해당하는 함수들을 모두 수행
			subscriber();
	}

	void Clear()
	{
		subscriber_groups.clear();
	}

private:
	EventSystem() = default;
	~EventSystem() = default;

private:
	std::map<EventType, std::vector<Subscriber>> subscriber_groups;
};