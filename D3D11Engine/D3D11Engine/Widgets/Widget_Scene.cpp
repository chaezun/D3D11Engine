#include "stdafx.h"
#include "Widget_Scene.h"
#include "ImGui/ImGuizmo.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Transform.h"

Widget_Scene::Widget_Scene(Context * context)
	:IWidget(context)
{
	title = "Scene";
	window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	timer = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();
}

void Widget_Scene::Render()
{
	ShowFrame();
	ShowTransformGizmo();
}

void Widget_Scene::ShowFrame()
{
	auto frame_position_x = static_cast<uint>(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x);
	auto frame_position_y = static_cast<uint>(ImGui::GetCursorPos().y + ImGui::GetWindowPos().y);

	auto frame_width = static_cast<uint>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto frame_height = static_cast<uint>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	frame_width -= frame_width % 2 != 0 ? 1 : 0;
	frame_height -= frame_height % 2 != 0 ? 1 : 0;

	renderer->SetEditorOffset(Vector2(static_cast<float>(frame_position_x), static_cast<float>(frame_position_y)));

	if (frame_counter >= 0.1f)
	{
		renderer->SetResolution(frame_width, frame_height);
		frame_counter = 0.0f;
	}
	frame_counter += timer->GetDeltaTimeSec();

	ImGui::Image
	(
		renderer->GetFrameResource(),
		ImVec2(static_cast<float>(frame_width), static_cast<float>(frame_height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(1, 1, 1, 1),
		ImVec4(1, 0, 0, 1)
	);	

	if(ImGui::IsMouseReleased(0) && ImGui::IsItemHovered())
	    Editor_Helper::Get().Picking();

	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
		Editor_Helper::Get().LoadModel(std::get<const char*>(payload->data));
}

void Widget_Scene::ShowTransformGizmo()
{
	if (Editor_Helper::Get().selected_actor.expired())
		return;

	auto camera = renderer->GetCamera();
	auto transform = Editor_Helper::Get().selected_actor.lock()->GetTransform();

	if (!camera || !transform)
		return;

	static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(87)) //W
		operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69)) //E
		operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) //R
		operation = ImGuizmo::SCALE;

	auto offset = renderer->GetEditorOffset();
	auto size = renderer->GetResolution();
	auto view = camera->GetViewMatrix().Transpose();
	auto proj = camera->GetProjectionMatrix().Transpose();
	auto world = transform->GetWorldMatrix().Transpose();

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(offset.x, offset.y, size.x, size.y);
	//Guizmo를 조정하는 함수
	ImGuizmo::Manipulate
	(
	  view,
	  proj,
	  operation,
	  mode,
	  world
	);

	world.Transpose();

	transform->SetTranslation(world.GetTranslation());
	transform->SetRotation(world.GetRotation());
	transform->SetScale(world.GetScale());
}
