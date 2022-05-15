#include "CLog.h"

namespace cwy {
CLog::CLog(const std::string& path)
    : filePath(GetPath(path)),
      logLevel(DEFAULT_LEVEL),
      fileSize(50),
      autoFlush(true)
{
}

CLog::~CLog()
{
    UnitLog();
}

void CLog::InitLog(const std::string& path, const LogLevel logLevel, const int fileSize, bool autoFlush)
{
    this->filePath = GetPath(path);
    this->logLevel = logLevel;
    this->fileSize = fileSize;
    this->autoFlush = autoFlush;
}

void CLog::UnitLog()
{
    if (autoFlush == false) {
        WriteFile(strReturn);
    }
}

void CLog::PrintlogDebug(const std::string& fileName, const int fileLine)
{
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(fileName, fileLine, "debug");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogInfo(const std::string& fileName, const int fileLine)
{
    if (logLevel > LogLevel::INFO_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(fileName, fileLine, "info");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogWarn(const std::string& fileName, const int fileLine)
{
    if (logLevel > LogLevel::WARN_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(fileName, fileLine, "warn");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogError(const std::string& fileName, const int fileLine)
{
    if (logLevel > LogLevel::ERROR_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(fileName, fileLine, "error");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

void CLog::PrintlogFatal(const std::string& fileName, const int fileLine)
{
    if (logLevel != LogLevel::FATAL_LEVEL) {
        strReturnOneLine = "";
        return;
    }
    std::lock_guard<std::mutex> mt(mtWriteFile);
    std::string log_write = AssembleString(fileName, fileLine, "fatal");
    if (autoFlush == false) {
        strReturn += log_write;
    } else {
        WriteFile(log_write);
    }
}

std::string CLog::AssembleString(const std::string& fileName, const int fileLine, const std::string& type)
{
    char temp[DATA_LENGTH] = { 0 };
    std::string temp_file_name(fileName);
    SYSTEMTIME st;
    GetLocalTime(&st);
    SplitFileName(temp_file_name);
    sprintf_s(temp, 1024 * 10, "[%02d:%02d:%02d %03d]: [%5d][%10s][%5d]\t[%s]%s\r\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, 
              GetCurrentThreadId(), temp_file_name.c_str(), fileLine, type.c_str(), strReturnOneLine.c_str());
    strReturnOneLine = "";
    return temp;
}

void CLog::SplitFileName(std::string& fileName)
{
    fileName = fileName.substr(fileName.find_last_of('\\') + 1);
}

void CLog::WriteFile(const std::string& content)
{
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

std::string CLog::GetPath(const std::string& filePath)
{
    std::string temp(filePath);
    std::string logPath = temp.substr(0, temp.find_last_of('/'));
    if (logPath.find("/") != std::string::npos) {
        return GetServicePath(temp) + ".log";
    } else {
        return GetClientPath(temp) + ".log";
    }
}

std::string CLog::GetServicePath(std::string& filePath)
{
    std::string logPath = filePath.substr(filePath.find_last_of('/'));
    std::string logPathForward = filePath.substr(0, filePath.find_last_of('/'));
    logPathForward = AssembleFilePath(logPathForward);
    return (logPathForward.substr(0, logPathForward.find_last_of(".")) + logPath);
}

std::string CLog::GetClientPath(std::string& filePath)
{
    std::string logPath = AssembleFilePath(filePath);
    return logPath;
}

std::string CLog::AssembleFilePath(std::string& filePath)
{
    char temp[MAX_PATH] = {0};
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf_s(temp, MAX_PATH, "%04d-%02d-%02d.txt", st.wYear, st.wMonth, st.wDay);
    int pos = filePath.find("{time}");
    if (pos != std::string::npos) {
        filePath.replace(pos, 6, temp);
        return filePath;
    } else {
        filePath = temp;
        return temp;
    }
}
}