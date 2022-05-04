#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <io.h>
#include <direct.h>
#include <string>
#include <mutex>

#include "public.h"

#ifdef _DEBUG
#define DEFAULT_LEVEL     LogLevel::DEBUG_LEVEL
#else
#define DEFAULT_LEVEL     logLevel::INFO_LEVEL
#endif

#define FILE_FORMAT                __FILE__ ,__LINE__
namespace cwy {
    enum class LogLevel {
        DEBUG_LEVEL = 0,
        INFO_LEVEL,
        WARN_LEVEL,
        ERROR_LEVEL,
        FATAL_LEVEL
    };

    class CLog {
    public:
        CLog() {};

        CLog(const std::string& path);

        ~CLog();

        void InitLog(const std::string& path, const LogLevel logLevel = DEFAULT_LEVEL, const int fileSize = 50, 
                     bool autoFlush = true);

        void UnitLog();

        void PrintlogDebug(const std::string& fileName, const int fileLine);

        void PrintlogInfo(const std::string& fileName, const int fileLine);

        void PrintlogWarn(const std::string& fileName, const int fileLine);

        void PrintlogError(const std::string& fileName, const int fileLine);

        void PrintlogFatal(const std::string& fileName, const int fileLine);

        friend CLog& operator<<(CLog& log, const std::string& a) {
            std::lock_guard<std::mutex> mt(log.mtWriteFile);
            log.strReturnOneLine += a;
            return log;
        }

        friend CLog& operator<<(CLog& log, const long long a) {
            std::lock_guard<std::mutex> mt(log.mtWriteFile);
            char temp[50] = { 0 };
            sprintf_s(temp, 50, "%lld", a);
            log.strReturnOneLine += temp;
            return log;
        }

    private:
        CLog(const CLog&) = delete;
        CLog(CLog&&) = delete;
        CLog& operator=(const CLog&) = delete;
        CLog& operator=(CLog&&) = delete;

        std::string AssembleString(const std::string& fileName, const int fileLine, const std::string& type);

        void SplitFileName(std::string& fileName);

        void WriteFile(const std::string& content);

        std::string GetPath(const std::string& filePath);

        std::string GetServicePath(std::string& filePath);

        std::string GetClientPath(std::string& filePath);

        std::string AssembleFilePath(std::string& filePath);

    private:
        std::mutex mtWriteFile;
        std::string strReturn;
        std::string strReturnOneLine;
        std::string filePath;
        LogLevel logLevel{DEFAULT_LEVEL};
        int fileSize{0};
        bool autoFlush{false};
        FILE* logFile{nullptr};
    };
}

static cwy::CLog logClient, logService;
#endif  //__MY_LOG_H__