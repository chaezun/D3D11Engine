#pragma once
#include "IWidget.h"
#include "Log/Engine_Logger.h"

class Widget_Console final : public IWidget
{
public:
	Widget_Console(class Context* context);
	~Widget_Console() = default;

	void Render() override;

	void AddLog(const Log_pair& log_pair);
	void ClearLog();

private:
	std::shared_ptr<Engine_Logger> logger;
	std::deque<Log_pair> logs;
	uint max_log_count = 500;
	bool is_show_info = true;
	bool is_show_warning = true;
	bool is_show_error = true;
};