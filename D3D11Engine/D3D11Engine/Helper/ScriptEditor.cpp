#include "stdafx.h"
#include "ScriptEditor.h"
#include "ImGui/TextEditor.h"

namespace ScriptEditor_Data
{
	static TextEditor editor;
	static TextEditor::ErrorMarkers markers;
	
	static const std::string base_script_code =
"class NewScript\n\
{\n\
\tNewScript (Actor@ in_actor)\n\
\t\{\n\
\t}\n\
\n\
\tvoid Start()\n\
\t\{\n\
\t}\n\
\n\
\tvoid Update()\n\
\t\{\n\
\t}\n\
}\n\
";
}

void ScriptEditor::Initialize(Context * context)
{
	this->context=context;
	ScriptEditor_Data::editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
	current_directory = Editor_Helper::Get().resource_manager->GetAssetDirectory() + "Script/";
	//SetScript("./ImGui/Source/imconfig.h");

	//auto language2 = TextEditor::LanguageDefinition::CPlusPlus();
	//{
	//	static const char* ppnames[] =
	//	{
	//		"NULL",
	//		"PM_REMOVE",
	//		"ZeroMemory",
	//		"DXGI_SWAP_EFFECT_DISCARD",
	//		"D3D_FEATURE_LEVEL",
	//		"D3D_DRIVER_TYPE_HARDWARE",
	//		"WINAPI",
	//		"D3D11_SDK_VERSION",
	//		"assert"
	//	};
	//
	//	static const char* ppvalues[] =
	//	{
	//		"#define NULL ((void*)0)",
	//		"#define PM_REMOVE (0x0001)",
	//		"Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ",
	//		"enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
	//		"enum D3D_FEATURE_LEVEL",
	//		"enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
	//		"#define WINAPI __stdcall",
	//		"#define D3D11_SDK_VERSION (7)",
	//		" #define assert(expression) (void)(                                                  \n"
	//		"    (!!(expression)) ||                                                              \n"
	//		"    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
	//		" )"
	//	};
	//
	//	for (int i = 0; i < ARRAYSIZE(ppnames); i++)
	//	{
	//		TextEditor::Identifier id;
	//		id.mDeclaration = ppvalues[i];
	//		language2.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
	//	}
	//
	//	static const char* identifiers[] =
	//	{
	//		"HWND", "HRESULT", "LPRESULT","D3D11_RENDER_TARGET_VIEW_DESC", "DXGI_SWAP_CHAIN_DESC","MSG","LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//		"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//		"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//		"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "TextEditor"
	//	};
	//
	//	static const char* idecls[] =
	//	{
	//		"typedef HWND_* HWND", "typedef long HRESULT", "typedef long* LPRESULT", "struct D3D11_RENDER_TARGET_VIEW_DESC", "struct DXGI_SWAP_CHAIN_DESC",
	//		"typedef tagMSG MSG\n * Message structure","typedef LONG_PTR LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//		"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//		"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//		"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "class TextEditor"
	//	};
	//
	//	for (int i = 0; i < ARRAYSIZE(identifiers); i++)
	//	{
	//		TextEditor::Identifier id;
	//		id.mDeclaration = std::string(idecls[i]);
	//		language2.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	//	}
	//}
	//ScriptEditor_Data::editor.SetLanguageDefinition(language2);
	//
	////SetScript("./ImGui/Source/imconfig.h");
	//SetScript("./ImGui/TextEditor.cpp");
	//
	//ScriptEditor_Data::markers.insert(std::make_pair(20, "Errrrrrrrrrrrrrrrrrrrrrrrrror"));
	//ScriptEditor_Data::markers.insert(std::make_pair(27, "Errrrrrrrrrrrrrrrrrrrrrrrrror"));
	//ScriptEditor_Data::markers.insert(std::make_pair(35, "Errrrrrrrrrrrrrrrrrrrrrrrrror"));
	//ScriptEditor_Data::editor.SetErrorMarkers(ScriptEditor_Data::markers);
}

void ScriptEditor::Render()
{
    if(!is_visible)
	   return;

	auto cursor_pos = ScriptEditor_Data::editor.GetCursorPosition();
	auto window_flags = ImGuiWindowFlags_NoDocking |
						ImGuiWindowFlags_HorizontalScrollbar |
						ImGuiWindowFlags_MenuBar;

	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Script Editor", &is_visible, window_flags);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				    //TODO:
					CreateNewScript(current_directory);

				if (ImGui::MenuItem("Open")) {}

				ImGui::Separator();

				if (ImGui::MenuItem("Save")) SaveToScript();
				if (ImGui::MenuItem("Save as...")) {}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				auto is_read_only = ScriptEditor_Data::editor.IsReadOnly();
				if (ImGui::MenuItem("ReadOnly Mode", nullptr, &is_read_only))
					ScriptEditor_Data::editor.SetReadOnly(is_read_only);

				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, !is_read_only && ScriptEditor_Data::editor.CanUndo()))
					ScriptEditor_Data::editor.Undo();

				if (ImGui::MenuItem("Redo", "Ctrl + Y", nullptr, !is_read_only && ScriptEditor_Data::editor.CanRedo()))
					ScriptEditor_Data::editor.Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, ScriptEditor_Data::editor.HasSelection()))
					ScriptEditor_Data::editor.Copy();

				if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, !is_read_only && ScriptEditor_Data::editor.HasSelection()))
					ScriptEditor_Data::editor.Cut();

				if (ImGui::MenuItem("Delete", "Delete", nullptr, !is_read_only && ScriptEditor_Data::editor.HasSelection()))
					ScriptEditor_Data::editor.Delete();

				if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, !is_read_only && ImGui::GetClipboardText() != nullptr))
					ScriptEditor_Data::editor.Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select All", "Ctrl + A", nullptr))
					ScriptEditor_Data::editor.SelectAll();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark"))
					ScriptEditor_Data::editor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light"))
					ScriptEditor_Data::editor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro Blue"))
					ScriptEditor_Data::editor.SetPalette(TextEditor::GetRetroBluePalette());
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Text
		(
			"%6d/%-6d %6d lines | %s | %s | %s | %s",
			cursor_pos.mLine + 1,
			cursor_pos.mColumn + 1,
			ScriptEditor_Data::editor.GetTotalLines(),
			ScriptEditor_Data::editor.IsOverwrite() ? "Ovr" : "Ins",
			ScriptEditor_Data::editor.CanUndo() ? "*" : " ",
			ScriptEditor_Data::editor.GetLanguageDefinition().mName.c_str(),
			path.c_str()
		);

		ScriptEditor_Data::editor.Render("Script Editor");
	}
	ImGui::End();
}

void ScriptEditor::SetScript(const std::string & path)
{
	this->path = path;

	std::ifstream in(path);
	if (in.good())
	{
		std::string str
		(
			(std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>()
		);

		ScriptEditor_Data::editor.SetText(str);
	}
}

void ScriptEditor::CreateNewScript(const std::string & directory)
{
	path = directory + "NewScript.as";

	//스크립트의 클래스 이름을 받아옴
	script_name = FileSystem::GetIntactFileNameFromPath(path);
	
	std::ofstream out;
	out.open(path, std::ios::out);
	if (out.fail())
		return;

	out << ScriptEditor_Data::base_script_code;
	out.flush();
	out.close();

	SetScript(path);
}

void ScriptEditor::SaveToScript()
{
	auto script_text = ScriptEditor_Data::editor.GetText();

	FileSystem::ReplaceAll(script_text, "\r", "");

	std::ofstream out;
	out.open(path, std::ios::out);
	if (out.fail())
		return;
    
	out << script_text;
	out.flush();
	out.close();
}

void ScriptEditor::ChangeScriptName(const std::string& path, const std::string& change_name)
{
	SetScript(path);

	//스크립트의 클래스 이름을 받아옴
	script_name = FileSystem::GetIntactFileNameFromPath(path);

	if (script_name == change_name)
	   return ;
	
    this->path=path;

	auto script_text = ScriptEditor_Data::editor.GetText();

	FileSystem::ReplaceAll(script_text, script_name, change_name);

	std::ofstream out;
	out.open(path, std::ios::out);
	if (out.fail())
		return ;

	out << script_text;
	out.flush();
	out.close();
}


