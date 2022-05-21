#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <string>
#include <queue>
#include <thread>

#include "CLog.h"
#define THREAD_CONTROL_UPDATE (WM_USER + 2000)

namespace cwy {
class ClientInfoTcp {
public:
    ClientInfoTcp()
        : communicationType(CommunicationType::NULLCOMMUNICATION), ip(), taskContent(), socket(INVALID_SOCKET)
    {
    }

    ClientInfoTcp(CommunicationType type, const std::string& taskIp, const std::string& content, SOCKET acceptSocket)
        : communicationType(type), ip(taskIp), taskContent(content), socket(acceptSocket)
    {
    }

    ClientInfoTcp& operator=(const ClientInfoTcp& other)
    {
        communicationType = other.GetType();
        ip = other.GetIp();
        taskContent = other.GetContent();
        socket = other.GetSocket();
        return *this;
    }

    void SetInfo(CommunicationType type, const std::string& taskIp, const std::string& content, SOCKET acceptSocket)
    {
        communicationType = type;
        ip = taskIp;
        taskContent = content;
        socket = acceptSocket;
    }

    CommunicationType GetType() const
    {
        return communicationType;
    }

    std::string GetIp() const
    {
        return ip;
    }

    std::string GetContent() const
    {
        return taskContent;
    }

    SOCKET GetSocket() const
    {
        return socket;
    }

    bool IsEmpty() const
    {
        return (socket == INVALID_SOCKET);
    }

private:
    CommunicationType communicationType;
    std::string ip;
    std::string taskContent;
    SOCKET socket;
};

static const unsigned short TCP_PORT = 6000;
static const unsigned short UDP_PORT = 6002;

static const unsigned short THREAD_NUM = 5;

class CNetWorkHandle {
public:
    CNetWorkHandle();

    ~CNetWorkHandle();

    std::string GetMainNetworkIp();

    void StartThread(HWND hWnd);

    void PushEvent(const ClientInfoTcp& clientInfoTcp);

    void ExitThread();
private:
    CNetWorkHandle(const CNetWorkHandle&) = delete;
    CNetWorkHandle(CNetWorkHandle&&) = delete;
    CNetWorkHandle& operator=(const CNetWorkHandle&) = delete;
    CNetWorkHandle& operator=(CNetWorkHandle&&) = delete;

    void HandleNetworkEvent(const unsigned short threadNo);

private:
    std::queue<ClientInfoTcp> taskQue;
    std::mutex taskMt;
    std::thread handleThread[THREAD_NUM];
    bool isExit{false};
    HWND hBackWnd{nullptr};
};

static ClientInfoTcp clientSendInfo;
}
#endif // !__NET_WORK_HANDLE_H__