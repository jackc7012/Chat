#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <tchar.h>
#include <queue>

#include "public.h"
#include "NetWorkHandle.h"

using namespace cwy;

const unsigned short TCP_PORT = 8888;
const unsigned short UDP_PORT = 6002;
const unsigned short THREAD_NUM = 5;

std::thread threadAcc, threadTcp, threadUdp;
std::vector<std::thread> threadHandle;
std::mutex mutexHandle, mutexPush;
std::queue<std::pair<std::string, SOCKET>> taskQue;
long long acceptCount{0};

SOCKET socketServiceTcp{0}, socketServiceUdp{0};
SOCKADDR_IN addrServiceTcp{0}, addrAccept{0}, addrServiceUdp{0};
std::vector<SOCKET> socketAccept;
std::unique_ptr<NetWorkHandle> netWorkHandle;

int HandleError(const std::string& msg)
{
    std::cout << msg << std::endl;
    system("pause");
    return -1;
}

void NetWorkEvent(const std::string& taskParam, const SOCKET socket)
{
    s_HandleRecv handleRecv;
    DecodeJson(taskParam, handleRecv);
    int ret = 0;
    switch (handleRecv.type_) {
        case CommunicationType::LOGIN:
        {
            netWorkHandle->HandleLogin(handleRecv);
            break;
        }
    }
    
    return ;
}

void ThreadHandler(const unsigned short threadNum)
{
    while (true) {
        std::string taskParam;
        SOCKET socket;
        {
            std::lock_guard<std::mutex> lg(mutexHandle);
            if (taskQue.empty()) {
                continue;
            }
            std::pair<std::string, SOCKET> param = taskQue.front();
            taskQue.pop();
            taskParam = param.first;
            socket = param.second;
            //std::cout << "thread" << threadNum << " process:: comunicationType = "
            //          << static_cast<unsigned int>(comunicationType) << " param = " << taskParam << std::endl;
        }
        NetWorkEvent(taskParam, socket);
    }
    return;
}

int InitThread()
{
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        threadHandle.push_back(std::thread(ThreadHandler, i));
    }
    return 0;
}

void HandleAcc()
{
    while (true) {
        SOCKADDR_IN addrClient = {0};
        int len = sizeof(SOCKADDR);
        SOCKET socket = ::accept(socketServiceTcp, (SOCKADDR*)&addrClient, &len);
        if (socket == SOCKET_ERROR) {
            continue;
        }
        {
            std::lock_guard<std::mutex> lg(mutexPush);
            socketAccept.push_back(socket);
            std::string ip = inet_ntoa(addrClient.sin_addr);
            ++acceptCount;
        }
    }
}

void HandleTcp()
{
    while (true) {
        char* strRecv = new char[DATA_LENGTH];
        memset(strRecv, 0, DATA_LENGTH);
        int ret = 0;
        unsigned int i = 0;
        for (; i < socketAccept.size(); ++i) {
            ret = ::recv(socketAccept[i], strRecv, DATA_LENGTH, 0);
            if (ret > 0)
                break;
        }
        if (ret > 0) {
            std::lock_guard<std::mutex> lg(mutexPush);
            taskQue.push(std::make_pair(strRecv, socketAccept[i]));
        }
        delete []strRecv;
    }
}

int StartTcp()
{
    socketServiceTcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketServiceTcp == INVALID_SOCKET) {
        return -1;
    } else {
        addrServiceTcp.sin_family = AF_INET;
        addrServiceTcp.sin_port = htons(TCP_PORT);
        addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
        if (::bind(socketServiceTcp, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR) {
            return -1;
        }
        if (::listen(socketServiceTcp, SOMAXCONN) == SOCKET_ERROR) {
            return -1;
        }
        threadAcc = std::thread(HandleAcc);
        threadTcp = std::thread(HandleTcp);
        threadAcc.join();
        threadTcp.join();
    }
    return 0;
}

int StartUdp()
{

    return 0;
}

int StartNetWork()
{
    char name[256] = {0};
    std::string result;
    int getNameRet = gethostname(name, sizeof(name));
    if (getNameRet == 0) {
        hostent* host = gethostbyname(name);
        if (NULL == host) {
            return HandleError("本地ip获取失败，程序将退出！");
        } else {
            std::cout << "本地监听ip：" << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;
        }
    } else {
        return HandleError("本地ip获取失败，程序将退出！");
    }
    StartTcp() && StartUdp();

    return 0;
}

int main()
{
    HANDLE chatServiceMutex = nullptr;
    chatServiceMutex = CreateMutex(nullptr, TRUE, "ChatService");
    if (chatServiceMutex == nullptr) {
        return HandleError("创建程序句柄失败，程序将退出！");
    } else if (chatServiceMutex != nullptr && GetLastError() == ERROR_ALREADY_EXISTS) {
        return HandleError("已经有聊天服务程序在运行，请关闭此程序！");
    }

    netWorkHandle = std::make_unique<NetWorkHandle>();
    if (netWorkHandle->InitDataBase("127.0.0.1", "MyChat") != 0) {
        return HandleError("数据库连接失败，程序将退出！");
    }
    InitThread();
    StartNetWork();

    system("pause");
    return 0;
}

