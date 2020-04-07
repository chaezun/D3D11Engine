#pragma once
#include "IWidget.h"

class Widget_ProgressBar final : public IWidget
{
public:
	Widget_ProgressBar(class Context* context);
	~Widget_ProgressBar()=default;

	void Begin() override;
	void Render() override;

private:
   std::string status = NOT_ASSIGNED_STR;
   float progress = 0.0f;
};