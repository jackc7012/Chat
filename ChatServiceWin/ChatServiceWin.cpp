#include <iostream>

#include "NetWorkHandle.h"

using namespace cwy;

const std::string infoFileName = "./info.ini";
std::unique_ptr<NetWorkHandle> netWorkHandle;

int EventHandle(const std::string& message, bool isError)
{
    std::cout << message << std::endl;
    logService_ << message;
    if (isError)
    {
        logService_.PrintlogError(FILE_FORMAT);
    }
    else
    {
        logService_.PrintlogInfo(FILE_FORMAT);
    }
    return (isError ? -1 : 0);
}

int main()
{
    do
    {
        std::string time = GetSystemTime(1);
        logService_.InitLog(std::string("./log/" + time + ".txt"));
        HANDLE chatServiceMutex = nullptr;
        chatServiceMutex = CreateMutex(nullptr, TRUE, "ChatService");
        if (chatServiceMutex == nullptr)
        {
            EventHandle("创建程序句柄失败，程序将退出!!!", true);
            break;
        }
        else if (chatServiceMutex != nullptr && GetLastError() == ERROR_ALREADY_EXISTS)
        {
            EventHandle("已经有聊天服务程序在运行，请关闭此程序!!!", true);
            break;
        }

        netWorkHandle = std::make_unique<NetWorkHandle>();
        if (netWorkHandle->Init(EventHandle, infoFileName) != 0)
        {
            EventHandle("程序初始化失败,请重试!!!", true);
            break;
        }
    } while (0);
    

    getchar();
    return 0;
}

