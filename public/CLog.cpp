#include <afx.h>
#include "CLog.h"

CLog::CLog() {
}

CLog::CLog(const std::string & path, const Log_Level log_level, bool auto_flush) :
    file_path(path)
    ,log_level(log_level)
    ,auto_flush(auto_flush) {

}

void CLog::InitLog(const std::string & path, const Log_Level log_level, bool auto_flush) {
    this->file_path = AssembleFilePath(path);
    this->log_level = log_level;
    this->auto_flush = auto_flush;
}

void CLog::UnitLog() {
    if (auto_flush == false) {
        WriteFile(str_return);
    }
}

CLog::~CLog() {
    if (auto_flush == false && file_path.compare("") != 0) {
        WriteFile(str_return);
    }
}

void CLog::PrintlogCustom(const std::string & file_name, const int file_line, const std::string &file_path/* = "./custom.txt"*/) {
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "debug");
    WriteFileCustom(ss, file_path);
}

void CLog::PrintlogInfo(const std::string & file_name, const int file_line) {
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "info");
    if (auto_flush == false) {
        str_return += ss;
    } else {
        WriteFile(ss);
    }
}

void CLog::PrintlogDebug(const std::string & file_name, const int file_line) {
    if (log_level > DEBUG_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "debug");
    if (auto_flush == false) {
        str_return += ss;
    } else {
        WriteFile(ss);
    }
}

void CLog::PrintlogWarn(const std::string & file_name, const int file_line) {
    if (log_level > WARN_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "warn");
    if (auto_flush == false) {
        str_return += ss;
    } else {
        WriteFile(ss);
    }
}

void CLog::PrintlogError(const std::string & file_name, const int file_line) {
    if (log_level > ERROR_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "error");
    if (auto_flush == false) {
        str_return += ss;
    } else {
        WriteFile(ss);
    }
}

void CLog::PrintlogFatal(const std::string & file_name, const int file_line) {
    if (log_level != FATAL_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string ss = AssembleString(file_name, file_line, "fatal");
    if (auto_flush == false) {
        str_return += ss;
    } else {
        WriteFile(ss);
    }
}

std::string CLog::AssembleFilePath(const std::string & file_path) {
    char temp[MAX_PATH] = { 0 };
    SYSTEMTIME st;
    GetTime(st);
    sprintf_s(temp, MAX_PATH, "%s-%04d-%02d-%02d.txt", file_path.c_str(), st.wYear, st.wMonth, st.wDay);
    return std::string(temp);
}

std::string CLog::AssembleString(const std::string & file_name, const int file_line, const std::string & type) {
    char temp[512] = { 0 };
    std::string temp_file_name(file_name);
    SYSTEMTIME st;
    GetSystemTime(&st);
    SplitFileName(temp_file_name);
    sprintf_s(temp, 512, "[%04d-%02d-%02d %02d:%02d:%02d %03d]: [%s]\t [%d][%s] %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, temp_file_name.c_str(), file_line, type.c_str(), str_return_one_line.c_str());
    str_return_one_line = "";
    return std::string(temp);
}

void CLog::GetTime(SYSTEMTIME &st) {
    GetSystemTime(&st);
}

void CLog::SplitFileName(std::string & file_name) {
    file_name = file_name.substr(file_name.find_last_of('\\') + 1);
}

void CLog::WriteFile(const std::string& content) {
    log_file.Open(file_path.c_str(), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
    log_file.SeekToEnd();
    log_file.Write(content.c_str(), content.length());
    log_file.Flush();
    log_file.Close();
}

void CLog::WriteFileCustom(const std::string & content, const std::string & file_path) {
    log_file.Open(file_path.c_str(), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
    log_file.SeekToEnd();
    log_file.Write(content.c_str(), content.length());
    log_file.Flush();
    log_file.Close();
}

CLog & operator<<(CLog & log_, const std::string & a) {
    log_.str_return_one_line += a;
    return log_;
}

CLog & operator<<(CLog & log_, const long long a) {
    char temp[50] = { 0 };
    sprintf_s(temp, 50, "%lld", (long long)a);
    log_.str_return_one_line += temp;
    return log_;
}