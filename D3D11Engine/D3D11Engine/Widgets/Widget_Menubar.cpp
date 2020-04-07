#include "stdafx.h"
#include "Widget_MenuBar.h"

Widget_MenuBar::Widget_MenuBar(Context * context)
	: IWidget(context)
{
}

void Widget_MenuBar::Render()
{
	is_show_script_editor = ScriptEditor::Get().IsVisible();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open")) {}

			ImGui::Separator();

			if (ImGui::MenuItem("Save")) {}
			if (ImGui::MenuItem("Save as...")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", nullptr, &is_show_metrics);
			ImGui::MenuItem("Style", nullptr, &is_show_style_editor);
			ImGui::MenuItem("Demo", nullptr, &is_show_demo);
			ImGui::MenuItem("Script Editor", nullptr, &is_show_script_editor);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	const auto style_editor = []()
	{
		ImGui::Begin("StyleEditor");
		ImGui::ShowStyleEditor();
		ImGui::End();
	};

	if (is_show_metrics)			 ImGui::ShowMetricsWindow();
	if (is_show_style_editor)		 style_editor();
	if (is_show_demo)				 ImGui::ShowDemoWindow();
	if (is_show_script_editor)       ScriptEditor::Get().SetVisible(true);
}

//Lamda
//람다식, 람다함수, 무명함수

// [변수캡쳐](파라미터) -> 리턴타입 { 함수 구현 }(넘길 인자)

//변수캡쳐  : 현재 함수에서 사용하는 외부 변수들
// = 을 사용하면 해당함수의 모든 변수 전부 사용
// & 을 사용하면 모든 변수를 참조로 받음
// 아무것도 안쓰면, 아무것도 않씀
// 전역변수는 캡쳐할 필요없음

//파라미터 : 함수에서 전달받을 인자 -> ex) int Add(int a, int b);
//리턴타입 :
//함수구현부 :
//넘길인자 :