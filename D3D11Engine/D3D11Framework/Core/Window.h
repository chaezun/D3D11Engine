#pragma once
#include "stdafx.h"

//namespace를 사용하는 이유: 중복때문에 생기는 충돌 방지
namespace Window
{
	//HINSTANCE: 프로그램의 실제 메모리를 가지고 있는
	//프로그램의 인스턴스 식별자(핸들)
	//윈도우가 실행중인 프로그램들을 구분하기 위한 식별값
	static HINSTANCE global_instance;

	//HWND: 프로그램 내에서의 윈도우 식별자(핸들)
	//하나의 프로그램이 많은 윈도우를 가질 수 있기 때문에 필요
	static HWND global_handle;

	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> editor_input_event; //에디터 모드
	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> game_input_event; //게임(플레이) 모드
	static std::function<void(const uint&, const uint&)> resize_event;

	//윈도우 메시지를 처리하는 함수
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
			if (resize_event && wParam != SIZE_MINIMIZED) //화면의 크기를 재조정했을 경우
				resize_event(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			//메인 윈도우가 파괴되면 응용 프로그램을 종료
			PostQuitMessage(0);
			break;

		default:
			//처리되지 않은 모든 메시지의 디폴트 처리를 수행
			//PostQuitMessage는 호출해 주지 않음
			return DefWindowProc(handle, message, wParam, lParam);
		}

		return 0;
	}

	//윈도우 창 생성
	inline void Create(HINSTANCE instance, const uint& width, const uint& height)
	{
		//윈도우 창 클래스 정의
		WNDCLASSEX wnd_class;
		//윈도우창 이외의 것을 사용할 것인지
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		//기본 윈도우창의 색상
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

		//CreateWindowExW 함수를 호출 할 때 사용할 창 클래스를 등록 
		auto check = RegisterClassEx(&wnd_class);
		assert(check != 0);

		//초기화된 윈도우 창 클래스의 내용을 토대로 윈도우창 생성
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

		//GetSystemMetrics : 컴퓨터 하드웨어에 따라 달라지거나 사용자에 의해서 재설정 가능한 정보들을
		//프로그램에서 얻어서 사용하고 싶을 때 사용
		//SM_CXSCREEN: 현재 화면 해상도의 x축 크기(pixel단위)를 얻음
		//SM_CYSCREEN: 현재 화면 해상도의 y축 크기(pixel단위)를 얻음
		uint x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		uint y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE); //윈도우의 크기를 설정하고 윈도우를 만들면 스크롤 바(오른쪽에 있는 바)가 제거된 상태의 윈도우가 만들어짐.

		MoveWindow(global_handle, x, y, rect.right - rect.left, rect.bottom - rect.top, FALSE); //마지막 인자가 FALSE이면 윈도우를 다시 그리지 않음.
	}

	inline void Show()
	{
		//띄운 창을 맨 앞으로 띄움
		SetForegroundWindow(global_handle);
		//띄운 창을 맨 앞으로 띄운 후 포커스 지정
		SetFocus(global_handle);
		ShowCursor(TRUE);
		ShowWindow(global_handle, SW_NORMAL);
		UpdateWindow(global_handle);
	}

	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		//GetMessage - 메세지가 올때까지 기다림(다른걸 처리하지 않음) (동기)
		//PeekMessage - 메세지가 안들어오면 그냥 넘어감 (비동기)

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//메세지 생성
			TranslateMessage(&msg);
			//WndProc으로 메세지 전달
			DispatchMessage(&msg);
		}

		//WM_QUIT는 PostQuitMessage(0)에서 설정됨
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