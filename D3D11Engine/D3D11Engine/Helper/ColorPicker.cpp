#include "stdafx.h"
#include "ColorPicker.h"

namespace ColorPicker_Data
{
	static std::string button_label;
	static std::string color_picker_label;
	static bool show_wheel         = false;
	static bool show_preview       = true;
	static bool hdr                = false;
	static bool alpha_preview      = true;
	static bool alpha_half_preview = true;
	static bool option_menu        = true;
	static bool show_RGB           = true;
	static bool show_HSV           = false;
	static bool show_HEX           = true;
}

ColorPicker::ColorPicker(const std::string & title)
   :title(title)
{
    ColorPicker_Data::button_label      = "##"+title;
	ColorPicker_Data::color_picker_label= "##"+title;
}

void ColorPicker::Update()
{
    if(ImGui::ColorButton(ColorPicker_Data::button_label.c_str(), ImVec4(color.r, color.g, color.b, color.a)))
	    is_visible=true;
 
    if(is_visible)
		ShowColorPicker();
}

void ColorPicker::ShowColorPicker()
{
    ImGui::SetWindowSize(ImVec2(400.0f,400.f), ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str(), &is_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowFocus();

	int misc_flags =
		(ColorPicker_Data::hdr ? ImGuiColorEditFlags_HDR : 0) |
		(ColorPicker_Data::alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (ColorPicker_Data::alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
		(ColorPicker_Data::option_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	ImGuiColorEditFlags						flags = misc_flags;
											flags |= ImGuiColorEditFlags_AlphaBar;
	if (!ColorPicker_Data::show_preview)    flags |= ImGuiColorEditFlags_NoSidePreview;
	if (ColorPicker_Data::show_wheel)       flags |= ImGuiColorEditFlags_PickerHueWheel;
	if (ColorPicker_Data::show_RGB)         flags |= ImGuiColorEditFlags_DisplayRGB;
	if (ColorPicker_Data::show_HSV)         flags |= ImGuiColorEditFlags_DisplayHSV;
	if (ColorPicker_Data::show_HEX)         flags |= ImGuiColorEditFlags_DisplayHex;

	ImGui::ColorPicker4(ColorPicker_Data::color_picker_label.c_str(), color, flags);
	ImGui::Separator();

	//Wheel
	ImGui::TextUnformatted("Wheel");
	ImGui::SameLine();
    ImGui::Checkbox("##ColorPicker_Wheel", &ColorPicker_Data::show_wheel);

	//RGB
	ImGui::TextUnformatted("RGB");
	ImGui::SameLine();
	ImGui::Checkbox("##ColorPicker_RGB", &ColorPicker_Data::show_RGB);

	//HSV
	ImGui::TextUnformatted("HSV");
	ImGui::SameLine();
	ImGui::Checkbox("##ColorPicker_HSV", &ColorPicker_Data::show_HSV);

	//HEX
	ImGui::TextUnformatted("HEX");
	ImGui::SameLine();
	ImGui::Checkbox("##ColorPicker_HEX", &ColorPicker_Data::show_HEX);

	ImGui::End();
}
