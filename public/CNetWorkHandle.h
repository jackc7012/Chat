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
    static const int TCP_PORT = 6000;
    static const int UDP_PORT = 6002;

    static const unsigned int THREAD_NUM = 5;

    struct ClientInfo {
        ClientInfo(const std::string& ip, const std::string& name, const std::string& token)
        :ip_(ip), name_(name), token_(token){}
        std::string ip_;
        std::string name_;
        std::string token_;
    };

    class CNetWorkHandle
    {
    public:
        static CNetWorkHandle* CreateInstance();

        ~CNetWorkHandle();

        bool InitNetWork(const HWND hWnd);

        bool ClientAccept(const SOCKET& socket, const SOCKADDR_IN& sockAddr);

        void HandleRecvTcp();
        void HandleRecvUdp();

    private:
        CNetWorkHandle();

        CNetWorkHandle(const CNetWorkHandle&) = delete;
        CNetWorkHandle operator=(const CNetWorkHandle&) = delete;

        void SetSocketInfo(const SOCKET& socket, const std::string& ip);

        CommunicationType HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend);

        void threadTask(int taskNum);

    private:
        CLog logNetWork_;
        CDataBase* dataBase_;
        std::vector<SOCKET> socketAccept_;
        std::unordered_map<SOCKET, ClientInfo> socketToClientInfo_;
        std::queue<s_TaskQueue> taskQueue_;
        std::thread myHandleThread_[THREAD_NUM];
        bool isExit_{ false };
        std::mutex mtServerHandle_;
        HWND mainWnd;
    };
}
#endif // !__NET_WORK_HANDLE_H__