#include "stdafx.h"
#include "FileDialog.h"

namespace FileDialog_Data
{
	static float item_size_min = 50.0f;
	static float item_size_max = 200.0f;
	static bool is_hovered_item = false;
	static bool is_hovered_window = false;
	static std::string hovered_item_path;
	static uint popup_id;
	static DragDropPayload payload;
}

#define OPERATION_NAME operation == FileDialogOperation::Open ? "Open" : operation == FileDialogOperation::Load ? "Load" : operation == FileDialogOperation::Save ? "Save" : "View"

FileDialog::FileDialog(Context * context, const FileDialogType & type, const FileDialogOperation & operation, const FileDialogFilter & filter, const bool & is_windowed)
	: context(context)
	, type(type)
	, operation(operation)
	, filter(filter)
	, is_windowed(is_windowed)
{
	title = OPERATION_NAME;
	current_directory = Editor_Helper::Get().resource_manager->GetAssetDirectory();
}

void FileDialog::SetOperation(const FileDialogOperation & operation)
{
	if (this->operation == operation)
		return;

	this->operation = operation;
	this->title = OPERATION_NAME;
}

auto FileDialog::Show(bool * is_visible, std::string * directory, std::string * full_path) -> const bool
{
	if (!(*is_visible))
	{
		is_update = true;
		return false;
	}

	is_selection = false;
	FileDialog_Data::is_hovered_item = false;
	FileDialog_Data::is_hovered_window = false;

	ShowTop(is_visible);
	ShowMiddle();
	ShowBottom(is_visible);

	if (is_windowed)
		ImGui::End();

	if (is_update)
	{
		UpdateDialogItemFromDirectory(current_directory);
		is_update = false;
	}

	if (is_selection)
	{
		if (directory)
			(*directory) = current_directory;

		if (full_path)
			(*full_path) = current_directory + "/" + input_box;
	}

	ShowMenuPopup();

	return is_selection;
}

void FileDialog::ShowTop(bool * is_visible)
{
	if (is_windowed)
	{
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints
		(
			ImVec2(350.0f, 250.0f),
			ImVec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
		);
		ImGui::Begin(title.c_str(), is_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDocking);
		ImGui::SetWindowFocus();
	}

	if (ImGui::ArrowButton("##Left_Button", ImGuiDir_Left))
	{
		SetDialogCurrentDirectory(FileSystem::GetParentDirectory(current_directory));
		is_update = true;
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(current_directory.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.8f);
	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.2f);
	{
		auto previous_width = item_size.x;
		ImGui::SliderFloat("##FileDialogSlider", &item_size.x, FileDialog_Data::item_size_min, FileDialog_Data::item_size_max);
		item_size.y += item_size.x - previous_width;
	}
	ImGui::PopItemWidth();
}

void FileDialog::ShowMiddle()
{
	const auto window = ImGui::GetCurrentWindowRead();
	const auto content_width = ImGui::GetContentRegionAvail().x;
	const auto content_height = ImGui::GetContentRegionAvail().y - (type == FileDialogType::Browser ? 30.0f : 0.0f);

	auto& g = *GImGui;
	auto& style = ImGui::GetStyle();
	const float font_scale = item_size.x / 100.0f;
	const float label_height = g.FontSize * font_scale;
	const float text_offset = 3.0f;
	auto check = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
	if (check = ImGui::BeginChild("##ContentRegion", ImVec2(content_width, content_height), true))
	{
		FileDialog_Data::is_hovered_window = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		auto pen_x = 0.0f;
		auto new_line = true;

		for (int i = 0; i < static_cast<int>(items.size()); i++)
		{
			if (new_line)
			{
				ImGui::BeginGroup();
				new_line = false;
			}

			ImGui::BeginGroup();
			{
				auto& item = items[i];
				auto button_pos_min = ImGui::GetCursorScreenPos();
				auto button_pos_max = ImVec2(button_pos_min.x + item_size.x, button_pos_min.y + item_size.y);

				//Tumbnail
				{
					ImGui::PushID(i);
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
					{
						if (ImGui::Button("##dummy", item_size))
						{
							item.Clicked();
							const auto is_single_click = item.GetTimeSinceLastClickedMs() > 500;

							if (is_single_click)
							{
								input_box = item.GetLabel();
								if (on_item_clicked)
									on_item_clicked(item.GetPath());
							}
							else
							{
								is_update = SetDialogCurrentDirectory(item.GetPath());
								is_selection = !item.IsDirectory();

								//더블클릭시 스크립트 화면 띄우기
								if (FileSystem::GetExtensionFromPath(item.GetPath()) == ".as")
								{
									ScriptEditor::Get().SetScript(item.GetPath());
									ScriptEditor::Get().SetVisible(true);
								}

								if (on_item_double_clicked)
									on_item_double_clicked(current_directory);
							}
						}

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
						{
							FileDialog_Data::is_hovered_item = true;
							FileDialog_Data::hovered_item_path = item.GetPath();
						}

						ItemClick(&item);
						ItemMenuPopup(&item);
						ItemDrag(&item);

						ImGui::SetCursorScreenPos(button_pos_min + style.FramePadding);
						ImGui::Image
						(
							item.GetTexture()->GetShaderResourceView(),
							ImVec2
							(
								button_pos_max.x - button_pos_min.x - style.FramePadding.x * 2.0f,
								button_pos_max.y - button_pos_min.y - style.FramePadding.y - label_height - 5.0f
							)
						);
					}
					ImGui::PopStyleColor(2);
					ImGui::PopID();
				}

				//Label
				{
					auto label_text = item.GetLabel().c_str();
					auto label_size = ImGui::CalcTextSize(label_text, nullptr, true);
					auto label_rect = ImRect
					(
						button_pos_min.x,
						button_pos_max.y - label_height - style.FramePadding.y,
						button_pos_max.x,
						button_pos_max.y
					);

					ImGui::GetWindowDrawList()->AddRectFilled
					(
						label_rect.Min,
						label_rect.Max,
						IM_COL32(255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.75f)
					);

					ImGui::SetWindowFontScale(font_scale);
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y + text_offset));
					if (label_size.x <= item_size.x)
						ImGui::TextUnformatted(label_text);
					else
						ImGui::RenderTextClipped
						(
							label_rect.Min,
							label_rect.Max,
							label_text,
							nullptr,
							&label_size,
							ImVec2(0, 0),
							&label_rect
						);

					if (is_change_script_name)
					{
						if (FileDialog_Data::popup_id == item.GetID())
						{
							ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y));
							ImGui::PushItemWidth(item_size.x);
							static std::string change_name = "";
							is_change =ImGui::InputText("", &change_name, ImGuiInputTextFlags_EnterReturnsTrue);
							if (ImGui::IsMouseClicked(1) && FileDialog_Data::is_hovered_window && !FileDialog_Data::is_hovered_item)
							{
								is_change_script_name = false;
								is_change = false;
							}
							if (is_change)
							{
								ScriptEditor::Get().ChangeScriptName(item.GetPath(), change_name);
								FileSystem::ChangeFileName(item.GetPath(), FileSystem::GetDirectoryFromPath(item.GetPath()) + change_name + FileSystem::GetExtensionFromPath(item.GetPath()));
								change_name = "";
								is_change_script_name = false;
								is_change = false;
								is_update = true;
							}
							ImGui::PopItemWidth();
						}
					}
				}
			}
			ImGui::EndGroup();

			pen_x += item_size.x + style.ItemSpacing.x;
			if (pen_x >= content_width - item_size.x)
			{
				ImGui::EndGroup();
				pen_x = 0;
				new_line = true;
			}
			else
				ImGui::SameLine();
		}

		if (!new_line)
			ImGui::EndGroup();

		ImGui::EndChild();
	}

	if (!check)
		ImGui::EndChild();

	ImGui::PopStyleVar();
}

void FileDialog::ShowBottom(bool * is_visible)
{
	if (type == FileDialogType::Browser)
		return;

	//TODO :
}

void FileDialog::ShowMenuPopup()
{
	if (ImGui::IsMouseClicked(1) && FileDialog_Data::is_hovered_window && !FileDialog_Data::is_hovered_item)
		ImGui::OpenPopup("##MenuPopup");

	if (!ImGui::BeginPopup("##MenuPopup"))
		return;

	if (ImGui::MenuItem("Create Folder"))
	{
		FileSystem::Create_Directory(current_directory + "New_Folder");
		is_update = true;
	}

	if (ImGui::MenuItem("File Explorer"))
		FileSystem::OpenDirectoryWindow(current_directory);

	ImGui::Separator();

	if (ImGui::MenuItem("Create Script"))
	{
		ScriptEditor::Get().CreateNewScript(ScriptEditor::Get().GetScriptDirectory());
		is_update = true;
	}

	ImGui::EndPopup();

}

void FileDialog::ItemDrag(FileDialogItem * item) const
{
	if (!item || type != FileDialogType::Browser)
		return;

	if (ImGui::BeginDragDropSource())
	{
		const auto set_payload = [](const PayloadType& type, const std::string& path)
		{
			FileDialog_Data::payload.type = type;
			FileDialog_Data::payload.data = path.c_str();
			DragDropEvent::CreateDragDropPayload(FileDialog_Data::payload);
		};

		if (FileSystem::IsSupportedTextureFile(item->GetPath()))    set_payload(PayloadType::Texture, item->GetPath());
		if (FileSystem::IsSupportedModelFile(item->GetPath()))     set_payload(PayloadType::Model, item->GetPath());
		if (FileSystem::IsSupportedScriptFile(item->GetPath()))     set_payload(PayloadType::Script, item->GetPath());
		if (FileSystem::IsSupportedAudioFile(item->GetPath()))      set_payload(PayloadType::Audio, item->GetPath());

		ImGui::Image(item->GetTexture()->GetShaderResourceView(), ImVec2(50.0f, 50.0f));

		ImGui::EndDragDropSource();
	}
}

void FileDialog::ItemClick(FileDialogItem * item) const
{
	if (!item || !FileDialog_Data::is_hovered_window || !FileDialog_Data::is_hovered_item)
		return;

	if (ImGui::IsItemClicked(1))
	{
		FileDialog_Data::popup_id = item->GetID();
		ImGui::OpenPopup("##ItemMenuPopup");
	}

}

void FileDialog::ItemMenuPopup(FileDialogItem * item)
{
	if (!item)
		return;

	if (FileDialog_Data::popup_id != item->GetID())
		return;

	if (!ImGui::BeginPopup("##ItemMenuPopup"))
		return;

	if (ImGui::MenuItem("Delete"))
	{
		//폴더
		if (item->IsDirectory())
		{
			FileSystem::Delete_Directory(item->GetPath());
			is_update = true;
		}

		//파일
		else
		{
			FileSystem::Delete_File(item->GetPath());
			is_update = true;
		}
	}

	ImGui::Separator();

	if (ImGui::MenuItem("File Explorer"))
		FileSystem::OpenDirectoryWindow(current_directory);

	if (FileSystem::GetExtensionFromPath(item->GetPath()) == ".as")
	{
		if (ImGui::MenuItem("Change Script Name"))
		{
			is_change_script_name = true;
		}

		else
		{
			is_change_script_name = false;
		}
	}

	ImGui::EndPopup();
}

auto FileDialog::SetDialogCurrentDirectory(const std::string & directory) -> const bool
{
	if (!FileSystem::IsDirectory(directory))
		return false;

	current_directory = directory;
	return true;
}

auto FileDialog::UpdateDialogItemFromDirectory(const std::string & directory) -> const bool
{
	if (!FileSystem::IsDirectory(directory))
	{
		LOG_ERROR("Invaild parameter");
		return false;
	}

	items.clear();
	items.shrink_to_fit();

	auto directories = FileSystem::GetDirectoriesInDirectory(directory);
	for (const auto& dir : directories)
		items.emplace_back(*Icon_Provider::Get().Load(dir, IconType::Thumbnail_Folder), dir);

	auto files = FileSystem::GetFilesInDirectory(directory);
	for (const auto& file : files)
		items.emplace_back(*Icon_Provider::Get().Load(file), file);

	return true;
}
