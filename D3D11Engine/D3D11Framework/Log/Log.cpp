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

    std::lock_guard<std::mutex> guard(log_mutex);

    const auto log_to_file      = logger.expired() || is_log_to_file;
    const auto formatted_text   = caller_name.empty() ? std::string(text) : caller_name + " : " + std::string(text);
    
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
    va_list args;
    va_start(args, text);
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);

    Write(buffer, LogType::Info);
}

void Log::Warning_f(const char * text, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);

    Write(buffer, LogType::Warning);
}

void Log::Error_f(const char * text, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, text);
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);

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

    const std::string prefix    = (type == LogType::Info) ? "Info : " : (type == LogType::Warning) ? "Warning : " : "Error : ";
    const auto final_text       = prefix + text;

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