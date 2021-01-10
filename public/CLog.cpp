#include "CLog.h"
using namespace cwy;

CLog::CLog(const std::string& path) {
    std::string ss = path;
    this->filePath = GetPath(ss);
    this->logLevel = DEFAULT_LEVEL;
    this->fileSize = 50;
    this->autoFlush = true;
}

CLog::~CLog() {
    UnitLog();
}
void CLog::InitLog(const std::string& path, const LogLevel log_level, const int file_size, bool auto_flush) {
    std::string ss = path;
    this->filePath = GetPath(ss);
    this->logLevel = log_level;
    this->fileSize = file_size;
    this->autoFlush = auto_flush;
}

void CLog::UnitLog() {
    if (autoFlush == false) {
        WriteFile(strReturn);
    }
}

void CLog::PrintlogInfo(const std::string& file_name, const int file_line) {
    if (logLevel > LogLevel::INFO_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(file_name, file_line, "info");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogDebug(const std::string& file_name, const int file_line) {
    if (logLevel > LogLevel::DEBUG_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(file_name, file_line, "debug");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogWarn(const std::string& file_name, const int file_line) {
    if (logLevel > LogLevel::WARN_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(file_name, file_line, "warn");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogError(const std::string& file_name, const int file_line) {
    if (logLevel > LogLevel::ERROR_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(file_name, file_line, "error");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogFatal(const std::string& file_name, const int file_line) {
    if (logLevel != LogLevel::FATAL_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(file_name, file_line, "fatal");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

std::string CLog::AssembleFilePath(std::string& file_path) {
    char temp[MAX_PATH] = { 0 };
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf_s(temp, MAX_PATH, "%04d-%02d-%02d.txt", st.wYear, st.wMonth, st.wDay);
    int pos = file_path.find("{time}");
    if (pos != std::string::npos) {
        file_path.replace(pos, 6, temp);
        return file_path;
    }
    else {
        file_path = temp;
        return std::string(temp);
    }
}

std::string CLog::AssembleString(const std::string& file_name, const int file_line, const std::string& type) {
    char temp[DATA_LENGTH] = { 0 };
    std::string temp_file_name(file_name);
    SYSTEMTIME st;
    GetLocalTime(&st);
    SplitFileName(temp_file_name);
    sprintf_s(temp, 512, "[%02d:%02d:%02d %03d]: [%d][%s][%d]\t[%s]%s\r\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, 
              GetCurrentThreadId(), temp_file_name.c_str(), file_line, type.c_str(), strReturnOneLine.c_str());
    strReturnOneLine = "";
    return std::string(temp);
}

void CLog::SplitFileName(std::string& file_name) {
    file_name = file_name.substr(file_name.find_last_of('\\') + 1);
}

void CLog::WriteFile(const std::string& content) {
    std::string logPath = filePath.substr(0, filePath.find_last_of('/'));
    if (_access(logPath.c_str(), 0) == -1) {
        _mkdir(logPath.c_str());
    }
    errno_t rt = fopen_s(&logFile, filePath.c_str(), "a+");
    if (rt == 0 && logFile != nullptr) {
        fwrite(content.c_str(), content.length(), 1, logFile);
        fclose(logFile);
    }  
}

std::string CLog::GetPath(std::string& filePath)
{
    std::string logPath = filePath.substr(0, filePath.find_last_of('/'));
    if (logPath.find("/") != std::string::npos) {
        return GetServicePath(filePath) + ".log";
    }
    else {
        return GetClientPath(filePath) + ".log";
    }
}

std::string CLog::GetServicePath(std::string& filePath)
{
    std::string logPath = filePath.substr(filePath.find_last_of('/'));
    std::string logPathForward = filePath.substr(0, filePath.find_last_of('/'));
    logPathForward = AssembleFilePath(logPathForward);
    return (logPathForward.substr(0, logPathForward.find_last_of(".")) + logPath);
}

std::string cwy::CLog::GetClientPath(std::string& filePath)
{
    std::string logPath = AssembleFilePath(filePath);
    return logPath;
}