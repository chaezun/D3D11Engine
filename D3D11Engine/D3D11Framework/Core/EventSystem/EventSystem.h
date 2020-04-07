#pragma once
#include "D3D11Framework.h"

//EventSystem : Engine�� �����Ҷ� �� Ŭ�������� ���� ������ �Ǵ��� �����ϱ� ���� ���. -> �̱������� ����
enum class EventType : uint
{
	Frame_Start, //Engine�� ó�� ������ ��
	Frame_End, //Engine�� ����� ��
	Update, //Engine�� ��� Update�� �ϴ� ���
	Render, //Engine�� �������� �ϴ� ���
	Scene_Refresh, //Scene�� ��ȯ�ϴ� ���
};

#define EVENT_HANDLER(function)      [this]() {function();} //�ش� Ŭ������ Ư���Լ� �����͸� ��ȯ(�����Լ� ���)

#define WORK_EVENT(event_type)         EventSystem::Get().Do(event_type) //�ش� EventType�� ��ϵ� �Լ��� ����

#define SUBSCRIBE_TO_EVENT(event_type, function) EventSystem::Get().Subscribe(event_type, function) //Event���
#define UNSUBSCRIBE_FROM_EVENT(event_type, function) EventSystem::Get().Unsubscribe(event_type, function) //Event��� ����

class EventSystem final
{
public:
	typedef std::function<void()> Subscriber; //�Լ������͸� ����Ͽ� �� EventType�� ������ �Լ����� ����

public:
	static EventSystem& Get() //�̱������� ��ü�� �޾ƿ��� ���� �Լ�
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
		if (subscriber_groups.find(event_type) == subscriber_groups.end()) //find() : �ش��ϴ� EventType�� �˻��ؼ� ��ã���� end()�� ��ȯ
		{
			return;
		}

		for (const auto& subscriber : subscriber_groups[event_type]) //EventType�� �ش��ϴ� �Լ����� ��� ����
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