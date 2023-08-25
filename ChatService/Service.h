#ifndef __MY_SERVICE_H__
#define __MY_SERVICE_H__

#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <functional>
#include "protocol.h"
#include "data_base.h"
#include "info.h"
#include "CLog.h"

static const std::string INFO_FILE_NAME = "./info.ini";

namespace cwy {
    typedef std::function<void(std::string, std::string, bool)> CallBack;

    const static unsigned int DEFAULT_REGISTER_ID = 9999;

    class Service {
    public:
        Service();

        ~Service();

        int Init(CallBack callBack);

        std::string GetServerIp();

        SOCKET StartTcp();

        void StopTcp();

        SOCKET ClientAccept();

        int ClientRecv(const SOCKET& socketClient);

        void NetWorkEvent(const unsigned short threadNum, const std::string& taskParam, const SOCKET socket);

    private:
        void SetEvent(CallBack callBack);

        void GetInfo();

        void InitLog();

        void InitThread();

        int InitDataBase();

        void ThreadHandler(const unsigned short threadNum);

        void HandleHeartBeat();

        std::string HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend);

        std::string HandleLogin(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend,
            bool& isLoginSucceed, std::string& customerName);

        void HandleExit(const UINT64 id);

        std::string HandleChat(const s_HandleRecv& handleRecv, const bool isOnline, s_HandleRecv& handleSend);

        std::string HandleShowLogin(const UINT64 id = -1, const int type = 0);

    private:
        CallBack callBack_{ nullptr };
        Info info_;
        INT64 maxRegistered_{ DEFAULT_REGISTER_ID }; // 当前注册id
        DataBaseHandle* dataBase_{ nullptr }; // 数据表处理
        std::vector<std::thread> threadHandle_; // 线程 : 处理消息
        HANDLE completionPort_{ nullptr };
        SOCKET socketServiceTcp_{ INVALID_SOCKET };
        SOCKADDR_IN addrServiceTcp_{ 0 };
        std::thread threadAcc_, threadTcp_, threadHeartBeat_; // 线程 : 接受socket, 处理消息压入队列, 心跳
        std::unordered_map<SOCKET, std::string> socketAccept_; // 所有连接未登录的socket socket号 : ip
        std::mutex mutexHandle_, mutexPush_;
        std::unordered_map<UINT64, std::pair<std::string, SOCKET>> loginCustomer_;  // 登录用户 id : {name : socket}
        std::queue<std::pair<std::string, SOCKET>> taskQue_; // 任务处理队列 消息 : socket号
        bool exitFlag{ false }; // 程序退出标志
        UINT64 loginCount_{ 0 }; // 登录人数
        CLog log_;
    };
}


#endif // ! __MY_SERVICE_H__