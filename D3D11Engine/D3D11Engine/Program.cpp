#include "stdafx.h"
#include "Editor.h"
#include "Core/Window.h"
#include "ImGui/imgui_impl_win32.h"

//Win32를 사용하므로 프로시저 핸들러를 선언해줘야 함.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, uint, WPARAM, LPARAM);

//실제로 D3D11Engine에 관련된 window 메세지를 처리함
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window::Create(hInstance, 1280, 720);
    Window::Show();

    Settings::Get().SetWindowInstance(hInstance);
    Settings::Get().SetWindowHandle(Window::global_handle);
    Settings::Get().SetWidth(static_cast<float>(Window::GetWidth()));
    Settings::Get().SetHeight(static_cast<float>(Window::GetHeight()));
	Settings::Get().SetVsync(false);

    auto editor = std::make_unique<Editor>();

	Window::editor_input_event = ImGui_ImplWin32_WndProcHandler;
	Window::game_input_event=Input::MouseProc;
	Window::resize_event       = [&editor](const uint& width, const uint& height)
	{
		if (editor)
		{
			editor->Resize(width,height);
			Settings::Get().SetWidth(static_cast<float>(width));
			Settings::Get().SetHeight(static_cast<float>(height));
	    }
	};

    while (Window::Update())
    {
        editor->Update();
        editor->Render();
    }

    Window::Destroy();
    return 0;
}