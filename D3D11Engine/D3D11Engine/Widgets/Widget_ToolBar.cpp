#include "stdafx.h"
#include "Widget_ToolBar.h"

Widget_ToolBar::Widget_ToolBar(Context * context)
	: IWidget(context)
{
	title = "ToolBar";
	window_flags |=
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoDocking;
}

void Widget_ToolBar::Begin()
{
	auto& ctx = *GImGui;
	ctx.NextWindowData.MenuBarOffsetMinVal = ImVec2
	(
		ctx.Style.DisplaySafeAreaPadding.x,
		ImMax(ctx.Style.DisplaySafeAreaPadding.y - ctx.Style.FramePadding.y, 0.0f)
	);

	ImGui::SetNextWindowPos(ImVec2(ctx.Viewports[0]->Pos.x, ctx.Viewports[0]->Pos.y + 23.9f));
	ImGui::SetNextWindowSize
	(
		ImVec2
		(
			ctx.Viewports[0]->Size.x,
			ctx.NextWindowData.MenuBarOffsetMinVal.y + ctx.FontBaseSize + ctx.Style.FramePadding.y + 20.0f
		)
	);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));
	ImGui::Begin(title.c_str(), &is_visible, window_flags);
}

void Widget_ToolBar::Render()
{
	//Play Button
	ImGui::SameLine();
	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[Engine::IsFlagEnabled(EngineFlags_Game) ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);

	if (Icon_Provider::Get().ImageButton(IconType::Button_Play, 20.0f))
	{ 
		Engine::FlagToggle(EngineFlags_Game);
	}

	ImGui::PopStyleColor();

	//Option Button
	ImGui::SameLine();
	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[is_show_options ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);

	if (Icon_Provider::Get().ImageButton(IconType::Button_Option, 20.0f))
	   is_show_options=true;

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	if(is_show_options)
	   ShowOptions();
}

void Widget_ToolBar::ShowOptions()
{
    auto renderer = Editor_Helper::Get().renderer;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, window_alpha);
	ImGui::Begin("Renderer Options", &is_show_options, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking);
	{
		ImGui::TextUnformatted("Opacity");
		ImGui::SameLine();
		ImGui::SliderFloat("##Opacity", &window_alpha, 0.1f, 1.0f, "%.1f");

		if (ImGui::CollapsingHeader("PostProcess", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const auto tool_tip = [](const char* text)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(text);
					ImGui::EndTooltip();
				}
			};

		   auto is_aabb = renderer->IsOnFlag(RendererOption_AABB);
		   auto is_grid = renderer->IsOnFlag(RendererOption_Grid);

		   ImGui::Checkbox("AABB", &is_aabb); tool_tip("Option to draw AABB for debugging");
		   ImGui::Checkbox("Grid", &is_grid); tool_tip("Option to draw Grid for debugging");

		   const auto set_options = [&](const RendererOption& option, const bool& is_enabled)
		   { 
		      is_enabled ? renderer->FlagEnable(option) : renderer->FlagDisable(option);
		   };

		   set_options(RendererOption_AABB, is_aabb);
		   set_options(RendererOption_Grid, is_grid);
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
