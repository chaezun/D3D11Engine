#pragma once
#include "stdafx.h"

//드래그해서 데이터를 특정 위치에 놓으면 그 데이터를 읽고 저장하는 플래그
enum class PayloadType : uint
{
	Unknown,
	Texture,
	Model,
	Audio,
	Script
};

struct DragDropPayload final
{
	typedef std::variant<const char*, uint> data_variant;

	DragDropPayload(const PayloadType& type = PayloadType::Unknown, const data_variant& data = nullptr)
		:type(type)
		, data(data)
	{

	}

	PayloadType type;
	data_variant data;
};

class DragDropEvent final
{
public:
	static void CreateDragDropPayload(const DragDropPayload& payload)
	{
		ImGui::SetDragDropPayload
		(
			reinterpret_cast<const char*>(&payload.type),
			reinterpret_cast<const void*>(&payload),
			sizeof(payload),
			ImGuiCond_Once
		);
	}

	static auto ReceiveDragDropPayload(const PayloadType& type) -> DragDropPayload*
	{
		if (ImGui::BeginDragDropTarget())
		{
		   if(const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
		      return static_cast<DragDropPayload*>(payload->Data);

		   ImGui::EndDragDropTarget();
	    }

		return nullptr;
	}
};