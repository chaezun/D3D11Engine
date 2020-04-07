#pragma once
#include "IWidget.h"

class Widget_Scene final : public IWidget
{
public:
	Widget_Scene(class Context* context);
	~Widget_Scene() = default;

	void Render() override;

private:
   void ShowFrame();
   void ShowTransformGizmo();

private:
   class Timer* timer=nullptr;
   class Renderer* renderer=nullptr;
   float frame_counter=0.0f;
};