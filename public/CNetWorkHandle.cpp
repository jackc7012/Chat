#include "CNetWorkHandle.h"

#include "NetWorkFactory.h"

namespace cwy {

CNetWorkHandle::CNetWorkHandle()
{
}

void CNetWorkHandle::HandleNetworkEvent(const unsigned short threadNo)
{
    while (true) {
        if (isExit) {
            break;
        }
        Sleep(1);
        std::string taskContent;
        if (!taskMt.try_lock()) {
            continue;
        }
        if (!taskQue.empty()) {
            taskContent = taskQue.front();
            taskQue.pop();
        }
        taskMt.unlock();
        if (!taskContent.empty()) {
            Json::Value jsValue;
            Json::Reader jsReader;
            jsReader.parse(taskContent, jsValue);
            if (!jsValue.isMember("communication_type")) {
                handle(-100, "error communication type");
                continue;
            }
            NetWorkFactory* networkFactory = new NetWorkFactory();
            NetWorkEvent *networkEvent = networkFactory->CreateNetWorkEvent(jsValue["communication_type"].asString());
            networkEvent->NetWorkEventHandle();
            delete networkEvent;
            delete networkFactory;
        }
    }
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
void CNetWorkHandle::PushEvent(const std::string& handleRecv)
{
    taskMt.lock();
    taskQue.push(handleRecv);
    taskMt.unlock();
}

void CNetWorkHandle::ExitThread()
{
    isExit = true;
}
}