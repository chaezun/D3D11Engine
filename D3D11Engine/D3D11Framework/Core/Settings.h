#pragma once
#include "D3D11Framework.h"

//윈도우 설정을 위한 클래스
class Settings final
{
public:
    static Settings& Get()
    {
        static Settings instance;
        return instance;
    }

	//Instance Setting
    auto GetWindowInstance() const -> HINSTANCE { return instance; }
    void SetWindowInstance(HINSTANCE instance) { this->instance = instance; }

	//Handle Setting
    auto GetWindowHandle() const -> HWND { return handle; }
    void SetWindowHandle(HWND handle) { this->handle = handle; }

	//Width Setting
    auto GetWidth() const -> const float& { return width; }
    void SetWidth(const float& width) { this->width = width; }

	//Height Setting
    auto GetHeight() const -> const float& { return height; }
    void SetHeight(const float& height) { this->height = height; }

	//수직동기화 Setting
	auto IsVsync() const -> const bool& { return is_vsync; }
	void SetVsync(const bool& is_vsync) { this->is_vsync = is_vsync; }

private:
    Settings() = default;
    ~Settings() = default;

private:
    HINSTANCE instance  = nullptr;
    HWND handle         = nullptr;
    float width         = 0.0f;
    float height        = 0.0f;
	bool is_vsync       = false;
};