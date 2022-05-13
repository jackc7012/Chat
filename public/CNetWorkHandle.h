#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <queue>
#include <thread>
#include <chrono>

#include "CLog.h"

namespace cwy {
    typedef std::function<void(int, std::string)> Fun;
    static const unsigned short TCP_PORT = 6000;
    static const unsigned short UDP_PORT = 6002;

    static const unsigned short THREAD_NUM = 5;

    struct ClientInfoTcp {
        ClientInfoTcp(const std::string& taskIp, std::string& content, SOCKET acceptSocket)
        :ip(taskIp), taskContent(content), socket(acceptSocket){}
        std::string ip;
        std::string taskContent;
        SOCKET socket;
    };

    class CNetWorkHandle
    {
    public:
        static CNetWorkHandle *CreateInstance();

        ~CNetWorkHandle();

        std::string GetMainNetworkIp();

        void StartThread(Fun handleAfter);

        void PushEvent(const std::string& handleRecv, const std::string& ip);

        void ExitThread();
    private:
        CNetWorkHandle(const CNetWorkHandle&) = delete;
        CNetWorkHandle(CNetWorkHandle&&) = delete;
        CNetWorkHandle& operator=(const CNetWorkHandle&) = delete;
        CNetWorkHandle& operator=(CNetWorkHandle&&) = delete;

        CNetWorkHandle();

        void HandleNetworkEvent(const unsigned short threadNo);

    private:
        Fun handle;
        std::queue<std::pair<std::string, std::string>> taskQue;
        std::mutex taskMt;
        std::thread handleThread[THREAD_NUM];
        bool isExit{false};
    };
}
#endif // !__NET_WORK_HANDLE_H__