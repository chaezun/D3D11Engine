#include "D3D11Framework.h"
#include "Log.h"
#include "ILogger.h"

std::string                     Log::caller_name;
bool                            Log::is_log_to_file = true;
std::weak_ptr<class ILogger>    Log::logger;
std::ofstream                   Log::f_out;
std::mutex                      Log::log_mutex;
std::string                     Log::log_file_name = "log.txt";
std::vector<LogCommand>         Log::log_buffers;
bool                            Log::is_first_log = true;

void Log::SetLogger(const std::weak_ptr<class ILogger>& i_logger)
{
	logger = i_logger;
}

void Log::Write(const char * text, const LogType & type)
{
	if (!text)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	//lock_guard 이 함수는 하나의 스레드만 log_mutex의 사용 권한을 가지도록 해줌
	//임계영역(한 번에 하나의 스레드만 사용가능)을 만들기 위해 사용
	std::lock_guard<std::mutex> guard(log_mutex);

	const auto log_to_file = logger.expired() || is_log_to_file;
	const auto formatted_text = caller_name.empty() ? std::string(text) : caller_name + " : " + std::string(text);

	if (log_to_file)
	{
		log_buffers.emplace_back(formatted_text, type);
		LogToFile(formatted_text.c_str(), type);
	}
	else
	{
		FlushBuffer();
		LogToLogger(formatted_text.c_str(), type);
	}

	caller_name.clear();
}

void Log::Write(const std::string & text, const LogType & type)
{
	Write(text.c_str(), type);
}

void Log::Info_f(const char * text, ...)
{
	char buffer[1024];
	va_list args; //가변인수 선언
	va_start(args, text); //스택 상의 첫 가변인수의 위치를 구해서 args에 대입. text는 고정 인수인데 가변인수의 시작번지(가변인수의 다음 번지)를 찾기 위해 필요.
	vsnprintf(buffer, sizeof(buffer), text, args); //sprintf와 비슷한 역할을 함(%d, %s, %c 등의 형식을 읽어 변환해주는 역할).
	va_end(args); //가변인수를 다 읽은 후, 함수가 정상적으로 리턴되도록 함. args가 변경되고 가변인수를 다시 읽을 수 없도록 함.

	Write(buffer, LogType::Info);
}

void Log::Warning_f(const char * text, ...)
{
	char buffer[1024];
	va_list args; //가변인수 선언
	va_start(args, text); //스택 상의 첫 가변인수의 위치를 구해서 args에 대입. text는 고정 인수인데 가변인수의 시작번지(가변인수의 다음 번지)를 찾기 위해 필요.
	vsnprintf(buffer, sizeof(buffer), text, args); //sprintf와 비슷한 역할을 함(%d, %s, %c 등의 형식을 읽어 변환해주는 역할).
	va_end(args); //가변인수를 다 읽은 후, 함수가 정상적으로 리턴되도록 함. args가 변경되고 가변인수를 다시 읽을 수 없도록 함.

	Write(buffer, LogType::Warning);
}

void Log::Error_f(const char * text, ...)
{
	char buffer[1024];
	va_list args; //가변인수 선언
	va_start(args, text); //스택 상의 첫 가변인수의 위치를 구해서 args에 대입. text는 고정 인수인데 가변인수의 시작번지(가변인수의 다음 번지)를 찾기 위해 필요.
	vsnprintf(buffer, sizeof(buffer), text, args); //sprintf와 비슷한 역할을 함(%d, %s, %c 등의 형식을 읽어 변환해주는 역할).
	va_end(args); //가변인수를 다 읽은 후, 함수가 정상적으로 리턴되도록 함. args가 변경되고 가변인수를 다시 읽을 수 없도록 함.

	Write(buffer, LogType::Error);
}

void Log::FlushBuffer()
{
	if (logger.expired() || log_buffers.empty())
		return;

	for (const auto& log : log_buffers)
		LogToLogger(log.text.c_str(), log.type);

	log_buffers.clear();
	log_buffers.shrink_to_fit();
}

void Log::LogToLogger(const char * text, const LogType & type)
{
	if (!text)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	logger.lock()->Log(std::string(text), static_cast<uint>(type));
}

void Log::LogToFile(const char * text, const LogType & type)
{
	if (!text)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	const std::string prefix = (type == LogType::Info) ? "Info : " : (type == LogType::Warning) ? "Warning : " : "Error : "; //메시지 서두에 LogType에 따라 미리 어떤 정보인지 적어둠.
	const auto final_text = prefix + text;

	if (is_first_log)
	{
		FileSystem::Delete_File(log_file_name);
		is_first_log = false;
	}

	f_out.open(log_file_name, std::ofstream::out | std::ofstream::app);

	if (f_out.is_open())
	{
		f_out << final_text << std::endl;
		f_out.close();
	}
}
