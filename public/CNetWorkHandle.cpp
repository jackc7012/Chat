#include "CNetWorkHandle.h"

#include "NetWorkEvent.h"

namespace cwy {

CNetWorkHandle::CNetWorkHandle()
{
}

void CNetWorkHandle::HandleNetworkEvent(const unsigned short threadNo)
{
    NetWorkEvent* networkEvent = new NetWorkEvent();
    taskMt.lock();
    networkEvent->InitDataBase("127.0.0.1", "MyChat");
    taskMt.unlock();
    while (true) {
        if (isExit) {
            break;
        }
        Sleep(1);
        std::string taskContent;
        std::string ip;
        if (!taskMt.try_lock()) {
            continue;
        }
        if (!taskQue.empty()) {
            taskContent = taskQue.front().first;
            ip = taskQue.front().second;
            taskQue.pop();
        }
        taskMt.unlock();
        if (!taskContent.empty()) {
            int code = 0;
            std::string msg;
            s_HandleRecv handleRecv;
            DecodeJson(taskContent, handleRecv);
            handleRecv.connect_ip_ = ip;
            networkEvent->NetWorkEventHandle(handleRecv, code, msg);
            handle(code, msg);
            UnregisterSpace(handleRecv.type_, handleRecv);
        }
    }
    delete networkEvent;
    networkEvent = nullptr;
}

CNetWorkHandle *CNetWorkHandle::CreateInstance()
{
    static CNetWorkHandle networkHandle;
    return &networkHandle;
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

void CNetWorkHandle::StartThread(Fun handleAfter)
{
    handle = handleAfter;
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        handleThread[i] = std::thread(&cwy::CNetWorkHandle::HandleNetworkEvent, this, i);
    }
}
void CNetWorkHandle::PushEvent(const std::string& handleRecv, const std::string& ip)
{
    taskMt.lock();
    taskQue.push(std::make_pair(handleRecv, ip));
    taskMt.unlock();
}

void CNetWorkHandle::ExitThread()
{
    isExit = true;
}
}