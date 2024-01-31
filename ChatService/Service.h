#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <thread>
#include <unordered_map>
#include <queue>
#include <functional>

#include "protocol.h"
#include "info.h"
#include "log.h"
#include "database.h"

namespace cwy
{
    static const std::string INFO_FILE_NAME = "./info.ini";

    enum class CallBackType
    {
        INIT,
        ACCEPT,
        RECV,
        SEND,
        LOGIN,
        LOGOUT
    };

    struct LoginPeople
    {
        LoginPeople(SOCKET socket, const std::string& name, const std::string ip)
            : socket_(socket), name_(name), ip_(ip)
        {
        }
        SOCKET socket_;
        std::string name_;
        std::string ip_;
    };

    typedef std::function<void(CallBackType, std::string, bool)> CallBack;

    const static UINT32 DEFAULT_REGISTER_ID = 9999;

    class Service
    {
    public:
        Service();

        ~Service();

        int Init(CallBack callBack);

        std::string GetServerIp();

        SOCKET StartTcp();

        void StopTcp();

        SOCKET ClientAccept();

        int ClientRecv(const SOCKET& socketClient);

        void NetWorkEvent(const UINT16 threadNum, const std::string& taskParam, const SOCKET socket);

        void HandleKick(const UINT64 id);

    private:
        void SetEvent(CallBack callBack);

        void GetInfo();

        void InitLog();

        void InitThread();

        int InitDataBase();

        void ThreadHandler(const UINT16 threadNum);

        void HandleHeartBeat();

        std::string HandleRegister(HandleRecv& handleRecv, const std::string& ip);

        std::string HandleLogin(HandleRecv& handleRecv, const std::string& ip, bool& isLoginSucceed, std::string& customerName);

        /*
        * type: 0  通知该用户 -> 当前所有用户
        *       1  通知其他用户 -> 该用户上线
        *       -1 通知其他用户 -> 该用户下线
        */
        std::string HandleShowLogin(const UINT64 id = -1, const std::string& customerName = "", const int type = 0);

        void HandleLogOut(const UINT64 id);

        /*
        * type: 0 文字
        *       1 图片/文件
        */
        void HandleChat(HandleRecv& handleRecv, const bool isOnline, const int type);

        std::string HandleChangePassword(HandleRecv& handleRecv);

        void HandleFileTransfer(HandleRecv& handleRecv);

    private:
        CallBack callBack_{ nullptr };
        Info info_;
        INT64 maxRegistered_{ DEFAULT_REGISTER_ID }; // 当前注册id
        DataBase* dataBase_{ nullptr }; // 数据表处理
        std::vector<std::thread> threadHandle_; // 线程 : 处理消息
        SOCKET socketServiceTcp_{ INVALID_SOCKET };
        SOCKADDR_IN addrServiceTcp_{ 0 };
        std::thread threadAcc_, threadTcp_, threadHeartBeat_; // 线程 : 接受socket, 处理消息压入队列, 心跳
        std::unordered_map<SOCKET, std::string> socketAccept_; // 所有连接未登录的socket socket号 : ip
        std::mutex mutexHandle_, mutexPush_;
        std::unordered_map<UINT64, LoginPeople> loginCustomer_;  // 登录用户 id : LoginPeople
        std::queue<std::pair<std::string, SOCKET>> taskQue_; // 任务处理队列 消息 : socket号
        bool exitFlag{ false }; // 程序退出标志
        UINT32 loginCount_{ 0 }; // 登录人数
        Log log_;
    };
}

#endif // ! __SERVICE_H__