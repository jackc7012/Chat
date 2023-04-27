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
    netWork_.tcpPort_ = GetPrivateProfileInt("network", "tcp_port", 6000, fileName.c_str());
    netWork_.udpPort_ = GetPrivateProfileInt("network", "udp_port", 6002, fileName.c_str());

    // database
    GetPrivateProfileString("database", "ip", "127.0.0.1", dataBase_.ip_, 50, fileName.c_str());
    GetPrivateProfileString("database", "name", "MyChat", dataBase_.name_, 50, fileName.c_str());
    GetPrivateProfileString("database", "uid", "sa", dataBase_.uid_, 50, fileName.c_str());
    GetPrivateProfileString("database", "pwd", "sa", dataBase_.pwd_, 50, fileName.c_str());

    // common
    common_.threadNum_ = GetPrivateProfileInt("common", "thread_num", 5, fileName.c_str());
}

const NetWork Info::GetNetWorkInfo() const
{
    return netWork_;
}

const DataBase Info::GetDataBaseInfo() const
{
    return dataBase_;
}

const Common Info::GetCommonInfo() const
{
    return common_;
}
