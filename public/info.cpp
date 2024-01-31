#include "info.h"

#include <Windows.h>

using namespace cwy;

Info::Info()
{
}

Info::~Info()
{
}

void Info::GetInfoFromFile(const std::string& fileName)
{
    // network
    GetPrivateProfileString("network", "ip", "127.0.0.1", netWork_.ip_, 50, fileName.c_str());
    netWork_.tcpPort_ = GetPrivateProfileInt("network", "tcp_port", 8888, fileName.c_str());
    netWork_.udpPort_ = GetPrivateProfileInt("network", "udp_port", 8890, fileName.c_str());

    // database
    GetPrivateProfileString("database", "ip", "127.0.0.1", dataBase_.ip_, 50, fileName.c_str());
    GetPrivateProfileString("database", "name", "MyChat", dataBase_.name_, 50, fileName.c_str());
    GetPrivateProfileString("database", "uid", "sa", dataBase_.uid_, 50, fileName.c_str());
    GetPrivateProfileString("database", "pwd", "sa", dataBase_.pwd_, 50, fileName.c_str());

    // log
    GetPrivateProfileString("log", "path", "./log.log", log_.path_, 50, fileName.c_str());
    log_.level_ = GetPrivateProfileInt("log", "level", 3, fileName.c_str());
    log_.size_ = GetPrivateProfileInt("log", "size", 50, fileName.c_str());
    log_.autoFlush_ = GetPrivateProfileInt("log", "autoFlush", 1, fileName.c_str());

    // common
    common_.threadNum_ = GetPrivateProfileInt("common", "thread_num", 5, fileName.c_str());
    GetPrivateProfileString("common", "file_address", "./file", common_.file_address_, 100, fileName.c_str());
}

const NetWorkInfo Info::GetNetWorkInfo() const
{
    return netWork_;
}

const DataBaseInfo Info::GetDataBaseInfo() const
{
    return dataBase_;
}

const LogInfo Info::GetLogInfo() const
{
    return log_;
}

const CommonInfo Info::GetCommonInfo() const
{
    return common_;
}
