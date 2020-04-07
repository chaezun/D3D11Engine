#include "stdafx.h"
#include "Widget_ProgressBar.h"
#include "Resource/ProgressReport.h"

Widget_ProgressBar::Widget_ProgressBar(Context * context)
:IWidget(context)
{
   title="Hold On.....";
   size=ImVec2(500.0f,83.0f);
   window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
   is_visible =false;
}

void Widget_ProgressBar::Begin()
{
  auto& report=ProgressReport::Get();
  auto is_model_loading=report.IsLoading(ProgressType::Model);

  if (is_model_loading)
  {
	  status   = report.GetStatus(ProgressType::Model);
	  progress = report.GetPrecentage(ProgressType::Model);
  }

  is_visible=is_model_loading;

  __super::Begin();  //IWidget::Begin()와 같은 의미

}

void Widget_ProgressBar::Render()
{
    if(!is_visible)
	   return;

    ImGui::SetWindowFocus();
	ImGui::PushItemWidth(500.0f-ImGui::GetStyle().WindowPadding.x*2.0f);
	ImGui::ProgressBar(progress);
	ImGui::TextUnformatted(status.c_str());
	ImGui::PopItemWidth();
}
