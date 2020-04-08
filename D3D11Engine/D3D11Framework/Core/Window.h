#pragma once
#include "stdafx.h"

//namespace�� ����ϴ� ����: �ߺ������� ����� �浹 ����
namespace Window
{
	//HINSTANCE: ���α׷��� ���� �޸𸮸� ������ �ִ�
	//���α׷��� �ν��Ͻ� �ĺ���(�ڵ�)
	//�����찡 �������� ���α׷����� �����ϱ� ���� �ĺ���
	static HINSTANCE global_instance;

	//HWND: ���α׷� �������� ������ �ĺ���(�ڵ�)
	//�ϳ��� ���α׷��� ���� �����츦 ���� �� �ֱ� ������ �ʿ�
	static HWND global_handle;

	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> editor_input_event; //������ ���
	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> game_input_event; //����(�÷���) ���
	static std::function<void(const uint&, const uint&)> resize_event;

	//������ �޽����� ó���ϴ� �Լ�
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
			if (resize_event && wParam != SIZE_MINIMIZED) //ȭ���� ũ�⸦ ���������� ���
				resize_event(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			//���� �����찡 �ı��Ǹ� ���� ���α׷��� ����
			PostQuitMessage(0);
			break;

		default:
			//ó������ ���� ��� �޽����� ����Ʈ ó���� ����
			//PostQuitMessage�� ȣ���� ���� ����
			return DefWindowProc(handle, message, wParam, lParam);
		}

		return 0;
	}

	//������ â ����
	inline void Create(HINSTANCE instance, const uint& width, const uint& height)
	{
		//������ â Ŭ���� ����
		WNDCLASSEX wnd_class;
		//������â �̿��� ���� ����� ������
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		//�⺻ ������â�� ����
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hInstance = instance;
		wnd_class.lpfnWndProc = static_cast<WNDPROC>(WndProc);
		wnd_class.lpszClassName = L"D3D11Engine";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		//CreateWindowExW �Լ��� ȣ�� �� �� ����� â Ŭ������ ��� 
		auto check = RegisterClassEx(&wnd_class);
		assert(check != 0);

		//�ʱ�ȭ�� ������ â Ŭ������ ������ ���� ������â ����
		global_handle = CreateWindowExW
		(
			WS_EX_APPWINDOW,
			L"D3D11Engine",
			L"D3D11Engine",
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

		//GetSystemMetrics : ��ǻ�� �ϵ��� ���� �޶����ų� ����ڿ� ���ؼ� �缳�� ������ ��������
		//���α׷����� �� ����ϰ� ���� �� ���
		//SM_CXSCREEN: ���� ȭ�� �ػ��� x�� ũ��(pixel����)�� ����
		//SM_CYSCREEN: ���� ȭ�� �ػ��� y�� ũ��(pixel����)�� ����
		uint x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		uint y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE); //�������� ũ�⸦ �����ϰ� �����츦 ����� ��ũ�� ��(�����ʿ� �ִ� ��)�� ���ŵ� ������ �����찡 �������.

		MoveWindow(global_handle, x, y, rect.right - rect.left, rect.bottom - rect.top, FALSE); //������ ���ڰ� FALSE�̸� �����츦 �ٽ� �׸��� ����.
	}

	inline void Show()
	{
		//��� â�� �� ������ ���
		SetForegroundWindow(global_handle);
		//��� â�� �� ������ ��� �� ��Ŀ�� ����
		SetFocus(global_handle);
		ShowCursor(TRUE);
		ShowWindow(global_handle, SW_NORMAL);
		UpdateWindow(global_handle);
	}

	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		//GetMessage - �޼����� �ö����� ��ٸ�(�ٸ��� ó������ ����) (����)
		//PeekMessage - �޼����� �ȵ����� �׳� �Ѿ (�񵿱�)

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//�޼��� ����
			TranslateMessage(&msg);
			//WndProc���� �޼��� ����
			DispatchMessage(&msg);
		}

		//WM_QUIT�� PostQuitMessage(0)���� ������
		return msg.message != WM_QUIT;
	}

	inline void Destroy()
	{
		DestroyWindow(global_handle);
		UnregisterClass(L"D3D11Engine", global_instance);
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