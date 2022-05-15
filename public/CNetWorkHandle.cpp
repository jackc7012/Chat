#include "CNetWorkHandle.h"

#include "NetWorkEvent.h"

namespace cwy {

CNetWorkHandle::CNetWorkHandle()
{
}

void CNetWorkHandle::HandleNetworkEvent(const unsigned short threadNo)
{
    std::unique_ptr<NetWorkEvent> networkEvent = std::make_unique<NetWorkEvent>();
    taskMt.lock();
    networkEvent->InitDataBase("127.0.0.1", "MyChat");
    taskMt.unlock();
    while (true) {
        if (isExit) {
            break;
        }
        Sleep(1);
        ClientInfoTcp clientInfoTcp;
        if (!taskMt.try_lock()) {
            continue;
        }
        if (!taskQue.empty()) {
            clientInfoTcp = taskQue.front();
            taskQue.pop();
        }
        taskMt.unlock();
        if (!clientInfoTcp.IsEmpty()) {
            CommunicationType type = CommunicationType::NULLCOMMUNICATION;
            std::string msg;
            s_HandleRecv handleRecv;
            handleRecv.connect_ip_ = clientInfoTcp.GetIp();
            handleRecv.socket_accept_ = clientInfoTcp.GetSocket();
            if (DecodeJson(clientInfoTcp.GetContent(), handleRecv) == false) {
                clientSendInfo.SetInfo(CommunicationType::ERRORCOMMUNICATION, handleRecv.connect_ip_,
                                       "", handleRecv.socket_accept_);
                PostMessage(hBackWnd, THREAD_CONTROL_UPDATE, (WPARAM)&clientSendInfo, 0);
            }
            networkEvent->NetWorkEventHandle(handleRecv, type, msg);
            clientSendInfo.SetInfo(type, handleRecv.connect_ip_, msg, handleRecv.socket_accept_);
            PostMessage(hBackWnd, THREAD_CONTROL_UPDATE, (WPARAM)&clientSendInfo, 0);
            UnregisterSpace(handleRecv.type_, handleRecv);
        }
    }
}

CNetWorkHandle::~CNetWorkHandle()
{
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        if (handleThread[i].joinable()) {
            handleThread[i].join();
        }
    }
}

std::string CNetWorkHandle::GetMainNetworkIp()
{
    char name[256] = {0};
    std::string result;
    int getNameRet = gethostname(name, sizeof(name));
    if (getNameRet == 0) {
        hostent* host = gethostbyname(name);
        if (NULL == host) {
            result = "get error";
        } else {
            result = inet_ntoa(*(in_addr*)*host->h_addr_list);
        }
    } else {
        result = "get error";
    }
    return result;
}

void CNetWorkHandle::StartThread(HWND hWnd)
{
    hBackWnd = hWnd;
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        handleThread[i] = std::thread(&cwy::CNetWorkHandle::HandleNetworkEvent, this, i);
    }
}
void CNetWorkHandle::PushEvent(const ClientInfoTcp& clientInfoTcp)
{
    taskMt.lock();
    taskQue.push(clientInfoTcp);
    taskMt.unlock();
}

void CNetWorkHandle::ExitThread()
{
    isExit = true;
}
}