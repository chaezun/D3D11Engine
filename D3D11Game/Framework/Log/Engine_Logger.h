#pragma once
#include "Framework.h"
#include "ILogger.h"

struct Log_pair final
{
	std::string text;
	int type;
};

class Engine_Logger final : public ILogger
{
public:
	typedef std::function<void(const Log_pair&)> log_function;

public:
	void SetCallBack(log_function&& function)
	{
		callback_function = std::forward<log_function>(function);
	}

	void Log(const std::string & log, const uint & type) override
	{
		Log_pair log_pair;
		log_pair.text = log;
		log_pair.type = type;

		callback_function(log_pair);
	}

private:
	log_function callback_function;
};

//  int a = 10;
//  lvalue  rvalue
//  &붙였을때 주소가 나오면 l벨류 안나오면 r벨류

// && -> R벨류참조
