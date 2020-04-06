#pragma once
#include "Framework.h"

class Settings final
{
public:
    static Settings& Get()
    {
        static Settings instance;
        return instance;
    }

    auto GetWindowInstance() const -> HINSTANCE { return instance; }
    void SetWindowInstance(HINSTANCE instance) { this->instance = instance; }

    auto GetWindowHandle() const -> HWND { return handle; }
    void SetWindowHandle(HWND handle) { this->handle = handle; }

    auto GetWidth() const -> const float& { return width; }
    void SetWidth(const float& width) { this->width = width; }

    auto GetHeight() const -> const float& { return height; }
    void SetHeight(const float& height) { this->height = height; }

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