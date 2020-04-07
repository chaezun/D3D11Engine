#pragma once
#include "stdafx.h"

namespace Window
{
	static HINSTANCE global_instance;
	static HWND global_handle;

	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> editor_input_event;
	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> game_input_event;
	static std::function<void(const uint&, const uint&)> resize_event;

	inline LRESULT CALLBACK WndProc(HWND handle, uint message, WPARAM wParam, LPARAM lParam)
	{
		if (editor_input_event != nullptr)
		{
			editor_input_event(handle, message, wParam, lParam);
		}

		if (game_input_event != nullptr)
		{
			game_input_event(handle, message, wParam, lParam);
		}

		switch (message)
		{
		case WM_DISPLAYCHANGE:
		case WM_SIZE:
			if (resize_event && wParam != SIZE_MINIMIZED)
				resize_event(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(handle, message, wParam, lParam);
		}

		return 0;
	}

	inline void Create(HINSTANCE instance, const uint& width, const uint& height)
	{
		WNDCLASSEX wnd_class;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hInstance = instance;
		wnd_class.lpfnWndProc = static_cast<WNDPROC>(WndProc);
		wnd_class.lpszClassName = L"D3DGame";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		auto check = RegisterClassEx(&wnd_class);
		assert(check != 0);

		global_handle = CreateWindowExW
		(
			WS_EX_APPWINDOW,
			L"D3DGame",
			L"D3DGame",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			instance,
			nullptr
		);
		assert(global_handle != nullptr);

		RECT rect{ 0,0,static_cast<LONG>(width),static_cast<LONG>(height) };

		uint x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		uint y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		MoveWindow(global_handle, x, y, rect.right-rect.left, rect.bottom-rect.top,TRUE);
	}

	inline void Show()
	{
		SetForegroundWindow(global_handle);
		SetFocus(global_handle);
		ShowCursor(TRUE);
		ShowWindow(global_handle, SW_NORMAL);
		UpdateWindow(global_handle);
	}

	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	inline void Destroy()
	{
		DestroyWindow(global_handle);
		UnregisterClass(L"D3DGame", global_instance);
	}

	inline const uint GetWidth()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<uint>(rect.right - rect.left);
	}

	inline const uint GetHeight()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<uint>(rect.bottom - rect.top);
	}
}