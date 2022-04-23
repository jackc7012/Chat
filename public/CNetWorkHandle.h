#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <windows.h>
#include <string>
#include <unordered_map>
#include <queue>
#include <thread>
#include <chrono>

#include "CDataBase.h"
#include "CLog.h"

namespace cwy {
    static const unsigned short TCP_PORT = 6000;
    static const unsigned short UDP_PORT = 6002;

    static const unsigned short THREAD_NUM = 5;

    struct ClientInfoTcp {
        ClientInfoTcp(const std::string& ip, const std::string& name, const std::string& token)
        :ip_(ip), name_(name), token_(token){}
        long long id_{0};
        std::string ip_;
        std::string name_;
        std::string token_;
    };

    class CNetWorkHandle
    {
    public:
        static CNetWorkHandle* CreateInstance();

        ~CNetWorkHandle();

        std::vector<std::string> InitNetWork(const HWND hWnd);

        void SetUdpSocket(SOCKET udpSocket);

        bool ClientAccept(const SOCKET& socket, const SOCKADDR_IN& sockAddr);

        void HandleRecvTcp();
        void HandleRecvUdp();

    private:
        CNetWorkHandle();

        CNetWorkHandle(const CNetWorkHandle&) = delete;
        CNetWorkHandle operator=(const CNetWorkHandle&) = delete;

        void SetSocketInfo(const SOCKET& socket, const std::string& ip);

        CommunicationType HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend);

        void threadTask(unsigned short taskNum);

    private:
        CLog logNetWork_;
        CDataBase* dataBase_;
        std::vector<SOCKET> socketAccept_;
        SOCKET udpSocket_;
        std::unordered_map<SOCKET, ClientInfoTcp> socketToClientInfoTcp_;
        std::unordered_map<long long, SOCKADDR_IN> idToSockaddrinUdp_;
        std::queue<s_TaskQueue> taskQueue_;
        std::thread myHandleThread_[THREAD_NUM];
        bool isExit_{ false };
        std::mutex mtServerHandle_;
        HWND mainWnd_;
    };
}
#endif // !__NET_WORK_HANDLE_H__