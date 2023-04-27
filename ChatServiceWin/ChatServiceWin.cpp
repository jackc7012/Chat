#include <iostream>
#include <tchar.h>

#include "NetWorkHandle.h"

using namespace cwy;

const std::string infoFileName = _T("./info.ini");
std::unique_ptr<NetWorkHandle> netWorkHandle;

int HandleError(const std::string& msg)
{
    std::cout << msg << std::endl;
    system("pause");
    return -1;
}

int EventHandle(const std::string& message, bool isError)
{
    std::cout << message << std::endl;
    return (isError ? -1 : 0);
}

int main()
{
    HANDLE chatServiceMutex = nullptr;
    chatServiceMutex = CreateMutex(nullptr, TRUE, "ChatService");
    if (chatServiceMutex == nullptr)
    {
        return HandleError("创建程序句柄失败，程序将退出！");
    }
    else if (chatServiceMutex != nullptr && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return HandleError("已经有聊天服务程序在运行，请关闭此程序！");
    }

    netWorkHandle = std::make_unique<NetWorkHandle>();
    netWorkHandle->Init(EventHandle, infoFileName);

    system("pause");
    return 0;
}

