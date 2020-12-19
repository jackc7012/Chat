#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <windows.h>
#include <string>
#include <mutex>

#include "public.h"

#ifdef _DEBUG
#define DEFAULT_LEVEL     Log_Level::DEBUG_LEVEL
#else
#define DEFAULT_LEVEL     Log_Level::ERROR_LEVEL
#endif

#define FILE_FORMAT                __FILE__ ,__LINE__
namespace mychat {
    enum class Log_Level {
        INFO_LEVEL = 0,
        DEBUG_LEVEL,
        WARN_LEVEL,
        ERROR_LEVEL,
        FATAL_LEVEL
    };

    class CLog {
    public:
        CLog();

        ~CLog();

        void InitLog(const std::string& path, const Log_Level log_level = DEFAULT_LEVEL, const int file_size = 50, 
                     bool auto_flush = true);

        void UnitLog();

        void PrintlogInfo(const std::string& file_name, const int file_line);

        void PrintlogDebug(const std::string& file_name, const int file_line);

        void PrintlogWarn(const std::string& file_name, const int file_line);

        void PrintlogError(const std::string& file_name, const int file_line);

        void PrintlogFatal(const std::string& file_name, const int file_line);

        friend CLog& operator<<(CLog& log, const std::string& a) {
            log.str_return_one_line += a;
            return log;
        }

        friend CLog& operator<<(CLog& log, const long long a) {
            char temp[50] = { 0 };
            sprintf_s(temp, 50, "%lld", (long long)a);
            log.str_return_one_line += temp;
            return log;
        }

    private:
        CLog(CLog&) = delete;
        CLog& operator=(CLog&) = delete;

        std::string AssembleFilePath(const std::string& file_path);

        std::string AssembleString(const std::string& file_name, const int file_line, const std::string& type);

        void SplitFileName(std::string& file_name);

        void WriteFile(const std::string& content);

    private:
        std::mutex mt_write_file;
        std::string str_return;
        std::string str_return_one_line;
        std::string file_path;
        Log_Level log_level{DEFAULT_LEVEL};
        int file_size{0};
        bool auto_flush{false};
        FILE* log_file{nullptr};
    };

}
#endif  //__MY_LOG_H__