#pragma once
#include "Framework.h"

enum class EventType : uint
{
	Frame_Start,
	Frame_End,
	Update,
	Render,
	Scene_Refresh,
};

#define EVENT_HANDLER_STATIC(function) []() {function();}
#define EVENT_HANDLER(function)        [this]() {function();}

#define FIRE_EVENT(event_type)         EventSystem::Get().Fire(event_type)

#define SUBSCRIBE_TO_EVENT(event_type, function) EventSystem::Get().Subscribe(event_type,function)
#define UNSUBSCRIBE_FROM_EVENT(event_type, function) EventSystem::Get().Unsubscribe(event_type,function)

class EventSystem final
{
public:
    typedef std::function<void()> Subscriber;

public:
	static EventSystem& Get()
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
	   const size_t function_address= *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&function));

	   auto& subscriber_group= subscriber_groups[event_type];
	   for (auto iter = subscriber_group.begin(); iter != subscriber_group.end();)
	   {
		   const size_t subscriber_address= *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&(*iter)));
		   if (subscriber_address == function_address)
		   {
			   iter=subscriber_group.erase(iter);
			   return;
		   }

		   else 
		      iter++;
	   }
	}

	void Fire(const EventType& event_type)
	{
		if (subscriber_groups.find(event_type) == subscriber_groups.end())
		{
		   return;
	    }

		for(const auto& subscriber: subscriber_groups[event_type])
			subscriber();
	}

	void Clear()
	{
		subscriber_groups.clear();
	}

private:
	EventSystem()=default;
	~EventSystem()= default;
 
 private:
    std::map<EventType,std::vector<Subscriber>> subscriber_groups;

};