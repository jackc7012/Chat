#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <windows.h>
#include <string>
#include <map>

#include "CDataBase.h"
#include "CLog.h"

namespace cwy {
    const int TCP_PORT = 6000;
    const int UDP_PORT = 6002;

    using IpName = std::pair<std::string, std::string>;

    class CNetWorkHandle
    {
    public:
        static CNetWorkHandle* CreateInstance();

        CommunicationType HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend);

        void SetSocketIp(const SOCKET& socket, const std::string& ip);

        ~CNetWorkHandle();

    private:
        CNetWorkHandle();

        CNetWorkHandle(const CNetWorkHandle&) = delete;
        CNetWorkHandle operator=(const CNetWorkHandle&) = delete;

    private:
        CLog logNetWork;
        CDataBase* dataBase;
        std::multimap<SOCKET, IpName> socketToIpName;
    };
}
#endif // !__NET_WORK_HANDLE_H__