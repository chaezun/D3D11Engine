#pragma once

enum class LogType : uint
{
	Info, //정보 출력
	Warning, //경고 출력
	Error, //에러 출력
};

struct LogCommand final
{
	LogCommand(const std::string& text, const LogType& type)
		: text(text)
		, type(type)
	{}

	std::string text; //출력할 메시지
	LogType type; //어떤 종류를 출력할지
};

//매크로 함수 사용
#define LOG_TO_FILE(value)          { Log::is_log_to_file = value; }

#define LOG_INFO(text)              { Log::caller_name = __FUNCTION__; Log::Write(text, LogType::Info); }
#define LOG_WARNING(text)           { Log::caller_name = __FUNCTION__; Log::Write(text, LogType::Warning); }
#define LOG_ERROR(text)             { Log::caller_name = __FUNCTION__; Log::Write(text, LogType::Error); }

#define LOG_INFO_F(text, ...)       { Log::caller_name = __FUNCTION__; Log::Info_f(text, __VA_ARGS__); }
#define LOG_WARNING_F(text, ...)    { Log::caller_name = __FUNCTION__; Log::Warning_f(text, __VA_ARGS__); }
#define LOG_ERROR_F(text, ...)      { Log::caller_name = __FUNCTION__; Log::Error_f(text, __VA_ARGS__); }

class Log final
{
public:
	static void SetLogger(const std::weak_ptr<class ILogger>& i_logger);

	static void Write(const char* text, const LogType& type);
	static void Write(const std::string& text, const LogType& type);

	template <typename T, typename = typename std::enable_if<
		std::is_same<T, int>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned int>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value>::type>
		static void Write(const T& value, const LogType& type)
	{
		Write(std::to_string(value), type);
	}

	static void Write(const bool& value, const LogType& type)
	{
		Write(value ? "True" : "Faluse", type);
	}

	//가변인자(...) 사용함수들 -> scanf와 printf와 비슷하게 구현
	static void Info_f(const char* text, ...);
	static void Warning_f(const char* text, ...);
	static void Error_f(const char* text, ...);

	static void FlushBuffer();
	static void LogToLogger(const char* text, const LogType& type);
	static void LogToFile(const char* text, const LogType& type); //Log를 파일로 전환하는 함수

public:
	static std::string caller_name;
	static bool is_log_to_file;

private:
	static std::weak_ptr<class ILogger> logger;
	static std::ofstream f_out;
	static std::mutex log_mutex;
	static std::string log_file_name;
	static std::vector<LogCommand> log_buffers;
	static bool is_first_log;
};