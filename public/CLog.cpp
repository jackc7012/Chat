#include "CLog.h"
using namespace mychat;

CLog* CLog::log = nullptr;

CLog* mychat::CLog::CreateInstance()
{
    if (log == nullptr) {
        log = new CLog;
    }
    return log;
}

void CLog::InitLog(const std::string &path, const Log_Level log_level, const int file_size, bool auto_flush) {
    this->file_path = AssembleFilePath(path);
    this->log_level = log_level;
    this->file_size = file_size;
    this->auto_flush = auto_flush;
}

void CLog::UnitLog() {
    if (auto_flush == false) {
        WriteFile(str_return);
    }
}

CLog::~CLog() {
    UnitLog();
}

void CLog::PrintlogInfo(const std::string& file_name, const int file_line) {
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string log_write = AssembleString(file_name, file_line, "info");
    if (auto_flush == false) {
        str_return += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogDebug(const std::string& file_name, const int file_line) {
    if (log_level < Log_Level::DEBUG_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string log_write = AssembleString(file_name, file_line, "debug");
    if (auto_flush == false) {
        str_return += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogWarn(const std::string& file_name, const int file_line) {
    if (log_level < Log_Level::WARN_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string log_write = AssembleString(file_name, file_line, "warn");
    if (auto_flush == false) {
        str_return += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogError(const std::string& file_name, const int file_line) {
    if (log_level < Log_Level::ERROR_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string log_write = AssembleString(file_name, file_line, "error");
    if (auto_flush == false) {
        str_return += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogFatal(const std::string& file_name, const int file_line) {
    if (log_level != Log_Level::FATAL_LEVEL) {
        str_return_one_line = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mt_write_file);
    std::string log_write = AssembleString(file_name, file_line, "fatal");
    if (auto_flush == false) {
        str_return += log_write;
    } else {
        WriteFile(log_write);
    }
}

std::string CLog::AssembleFilePath(const std::string& file_path) {
    char temp[MAX_PATH] = { 0 };
    SYSTEMTIME st;
    GetSystemTime(&st);
    sprintf_s(temp, MAX_PATH, "%s-%04d-%02d-%02d.txt", file_path.substr(0, file_path.find_last_of('.')).c_str(), 
                    st.wYear, st.wMonth, st.wDay);
    return std::string(temp);
}

std::string CLog::AssembleString(const std::string& file_name, const int file_line, const std::string& type) {
    char temp[data_length] = { 0 };
    std::string temp_file_name(file_name);
    SYSTEMTIME st;
    GetSystemTime(&st);
    SplitFileName(temp_file_name);
    sprintf_s(temp, 512, "[%02d:%02d:%02d %03d]: [%s]\t [%d][%s] %s\r\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,                    temp_file_name.c_str(), file_line, type.c_str(), str_return_one_line.c_str());
    str_return_one_line = "";
    return std::string(temp);
}

void CLog::SplitFileName(std::string &file_name) {
    file_name = file_name.substr(file_name.find_last_of('\\') + 1);
}

void CLog::WriteFile(const std::string& content) {
    errno_t rt = fopen_s(&log_file, file_path.c_str(), "a+");
    if (rt == 0 && log_file != nullptr) {
        fwrite(content.c_str(), content.length(), 1, log_file);
        fclose(log_file);
    }  
}