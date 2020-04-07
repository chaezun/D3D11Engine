#include "stdafx.h"
#include "Widget_Project.h"
#include "Helper/FileDialog.h"

namespace Project_Data
{
	static bool is_show_file_dialog_view = true;
	static std::string double_clicked_path;
}

Widget_Project::Widget_Project(Context * context)
	: IWidget(context)
{
	title = "Project";
	window_flags |= ImGuiWindowFlags_NoScrollbar;

	file_dialog_view = std::make_unique<FileDialog>(context, FileDialogType::Browser, FileDialogOperation::Load, FileDialogFilter::All, false);
}

Widget_Project::~Widget_Project() = default;

void Widget_Project::Render()
{
	if (!is_visible)
		return;

	file_dialog_view->Show(&Project_Data::is_show_file_dialog_view);
}
