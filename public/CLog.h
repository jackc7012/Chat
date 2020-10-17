#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <windows.h>
#include <string>
#include <mutex>

namespace mywork {
#define FILE_FORMAT                __FILE__ ,__LINE__

#define endli                     "info"
#define endld                     "debug"
#define endlw                     "warn"
#define endle                     "error"
#define endlf                     "fatal"

    enum Log_Level {
        INFO_LEVEL = 0,
        DEBUG_LEVEL,
        WARN_LEVEL,
        ERROR_LEVEL,
        FATAL_LEVEL
    };

    class CLog {
    public:
        CLog();

        CLog(const std::string& path, const Log_Level log_level, bool auto_flush = false);

        void InitLog(const std::string& path, const Log_Level log_level, bool auto_flush = false);

        void UnitLog();

        ~CLog();

        void PrintlogCustom(const std::string& file_name, const int file_line, const std::string& file_path = "./custom.txt");

        void PrintlogInfo(const std::string& file_name, const int file_line);

        void PrintlogDebug(const std::string& file_name, const int file_line);

        void PrintlogWarn(const std::string& file_name, const int file_line);

        void PrintlogError(const std::string& file_name, const int file_line);

        void PrintlogFatal(const std::string& file_name, const int file_line);

        friend CLog& operator<<(CLog& log_, const std::string& a);

        friend CLog& operator<<(CLog& log_, const long long a);

    private:
        std::string AssembleFilePath(const std::string& file_path);

        std::string AssembleString(const std::string& file_name, const int file_line, const std::string& type);

        void GetTime(SYSTEMTIME& st);

        void SplitFileName(std::string& file_name);

        void WriteFile(const std::string& content);

        void WriteFileCustom(const std::string& content, const std::string& file_path);

    private:
        std::mutex mt_write_file;
        std::string str_return;
        std::string str_return_one_line;
        std::string file_path;
        Log_Level log_level;
        bool auto_flush;
        CFile log_file;
    };
}

#endif  //__MY_LOG_H__