#pragma once
#include "D3D11Framework.h"
#include "ILogger.h"

struct Log_pair final
{
	std::string text;
	int type;
};

//실제로 엔진에서 로그를 찍을 클래스
class Engine_Logger final : public ILogger
{
public:
	typedef std::function<void(const Log_pair&)> log_function;

public:
	void SetCallBack(log_function&& function) //실제로 함수포인터를 사용해서 log를 추가하는 함수
	{
		callback_function = std::forward<log_function>(function);
	}

	void Log(const std::string& log, const uint& type) override
	{
		Log_pair log_pair;
		log_pair.text = log;
		log_pair.type = type;

		callback_function(log_pair);
	}

private:
	log_function callback_function;
};