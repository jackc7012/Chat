#ifndef __NETWORKHANDLE_H__
#define __NETWORKHANDLE_H__

#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include "protocol.h"
#include "data_base.h"
#include "info.h"

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

        int InitThread();

        int StartNetWork();

        int StartTcp();

        int GetInfo(const std::string& infoFileName);

        int InitDataBase();

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
        std::unique_ptr<DataBaseHandle> dataBase_{ (std::make_unique<DataBaseHandle>()) };
        std::vector<std::thread> threadHandle_;
        CallBack callBack_{ nullptr };
        SOCKET socketServiceTcp_{ 0 };
        SOCKADDR_IN addrServiceTcp_{ 0 }, addrAccept_{ 0 };
        std::vector<SOCKET> socketAccept_;
        Info info_;
        std::thread threadAcc_, threadTcp_, threadHeartBeat_;
        std::mutex mutexHandle_, mutexPush_;
        // id : {name : socket}
        std::unordered_map<UINT64, std::pair<std::string, SOCKET>> loginCustomer_;
        std::queue<std::pair<std::string, SOCKET>> taskQue_;
        UINT64 loginCount_{ 0 };
        INT64 maxRegistered_{ DEFAULT_REGISTER_ID };
        bool exitFlag{ false };
    };
}

#endif //!__NETWORKHANDLE_H__