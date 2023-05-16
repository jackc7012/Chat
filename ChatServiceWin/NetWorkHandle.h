#ifndef __MY_NETWORKHANDLE_H__
#define __MY_NETWORKHANDLE_H__

#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include "protocol.h"
#include "data_base.h"
#include "info.h"
#include "CLog.h"

namespace cwy {
    typedef int (*CallBack)(const std::string& message, bool isError);

    const static unsigned int DEFAULT_REGISTER_ID = 9999;

    class NetWorkHandle {
    public:
        NetWorkHandle();

        ~NetWorkHandle();

        int Init(CallBack callBack, const std::string& infoFileName);

    private:
        int SetEvent(CallBack callBack);

        int GetInfo(const std::string& infoFileName);

        int InitThread();

        int StartNetWork();

        int InitDataBase();

        int StartTcp();

        void ThreadHandler(const unsigned short threadNum);

        void HandleAcc();

        void HandleTcp();

        void HandleHeartBeat();

        void NetWorkEvent(const std::string& taskParam, const SOCKET socket);

        std::string HandleRegister(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend);

        std::string HandleLogin(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend,
            bool& isLoginSucceed, std::string& customerName);

        void HandleExit(const UINT64 id);

        std::string HandleChat(const s_HandleRecv& handleRecv, const bool isOnline, s_HandleRecv& handleSend);

        std::string HandleShowLogin(const UINT64 id = -1, const int type = 0);

        inline int JudgeCallBack() const
        {
            if (callBack_ == nullptr)
            {
                return -1;
            }
            return 0;
        }

    private:
        std::unique_ptr<DataBaseHandle> dataBase_{ std::make_unique<DataBaseHandle>() }; // 数据表处理
        std::thread threadAcc_, threadTcp_, threadHeartBeat_; // 线程: 接受socket, 处理消息压入队列, 心跳
        std::vector<std::thread> threadHandle_; // 线程:处理消息
        CallBack callBack_{ nullptr }; // 回调
        SOCKET socketServiceTcp_{ INVALID_SOCKET };
        std::vector<SOCKET> socketAccept_; // 所有接受的socket
        Info info_; // 配置文件信息
        std::mutex mutexHandle_, mutexPush_;
        std::unordered_map<UINT64, std::pair<std::string, SOCKET>> loginCustomer_;  // 登录用户 id : {name : socket}
        std::queue<std::pair<std::string, SOCKET>> taskQue_; // 任务处理队列 消息 : socket号
        UINT64 loginCount_{ 0 }; // 登录人数
        INT64 maxRegistered_{ DEFAULT_REGISTER_ID }; // 当前注册id
        bool exitFlag{ false }; // 程序退出标志
    };
}

#endif //!__MY_NETWORKHANDLE_H__