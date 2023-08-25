#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <io.h>
#include <direct.h>
#include <string>
#include <mutex>

#include "public.h"

#ifdef _DEBUG
#define DEFAULT_LEVEL     LogLevel::INFO_LEVEL
#else
#define DEFAULT_LEVEL     Log_Level::ERROR_LEVEL
#endif

#define FILE_FORMAT                __FILE__, __LINE__
namespace cwy {
    enum class LogLevel {
        INFO_LEVEL = 0,
        DEBUG_LEVEL,
        WARN_LEVEL,
        ERROR_LEVEL,
        FATAL_LEVEL
    };

    class CLog {
    public:
        CLog() {};

        CLog(const std::string& path);

        ~CLog();

        void InitLog(const int type, const std::string& path, const LogLevel log_level = DEFAULT_LEVEL, const int file_size = 50,
            const bool auto_flush = true);

        void UnitLog();

        void PrintlogInfo(const std::string& file_name, const int file_line);

        void PrintlogDebug(const std::string& file_name, const int file_line);

        void PrintlogWarn(const std::string& file_name, const int file_line);

        void PrintlogError(const std::string& file_name, const int file_line);

        void PrintlogFatal(const std::string& file_name, const int file_line);

        friend CLog& operator<<(CLog& log, const std::string& a)
        {
            std::lock_guard<std::mutex> mt(log.mtWriteFile);
            log.strReturnOneLine += a;
            return log;
        }

        friend CLog& operator<<(CLog& log, const long long a)
        {
            std::lock_guard<std::mutex> mt(log.mtWriteFile);
            char temp[50] = { 0 };
            sprintf_s(temp, 50, "%lld", (long long)a);
            log.strReturnOneLine += temp;
            return log;
        }

    private:
        std::string AssembleFilePath(std::string& file_path);

        std::string AssembleString(const std::string& file_name, const int file_line, const std::string& type);

        void SplitFileName(std::string& file_name);

        void WriteFile(const std::string& content);

        std::string GetPath(std::string& filePath);

        std::string GetServicePath(std::string& filePath);

        std::string GetClientPath(std::string& filePath);

    private:
        std::mutex mtWriteFile;
        std::string strReturn;
        std::string strReturnOneLine;
        std::string filePath;
        LogLevel logLevel{ DEFAULT_LEVEL };
        int fileSize{ 0 };
        bool autoFlush{ false };
        FILE* logFile{ nullptr };
    };
}

static cwy::CLog logClient_, logService_;
#endif  //__MY_LOG_H__