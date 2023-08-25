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

    class NetWorkHandle {
    public:
        NetWorkHandle();

        NetWorkHandle(const Info& info)
            : info_(info)
        {

        }

        ~NetWorkHandle();

        int Init(CallBack callBack = nullptr, const std::string& infoFileName = "");


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

        std::string HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend);

        std::string HandleLogin(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend,
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
        UINT64 loginCount_{ 0 }; // ��¼����
        INT64 maxRegistered_{ 9 }; // ��ǰע��id
        bool exitFlag{ false }; // �����˳���־

        std::unique_ptr<DataBaseHandle> dataBase_{ std::make_unique<DataBaseHandle>() }; // ���ݱ���
        std::thread threadAcc_, threadTcp_, threadHeartBeat_; // �߳� : ����socket, ������Ϣѹ�����, ����
        std::vector<std::thread> threadHandle_; // �߳� : ������Ϣ
        std::unordered_map<SOCKET, std::string> socketAccept_; // ���н��ܵ�socket socket�� : ip
        std::mutex mutexHandle_, mutexPush_;
        std::unordered_map<UINT64, std::pair<std::string, SOCKET>> loginCustomer_;  // ��¼�û� id : {name : socket}
        std::queue<std::pair<std::string, SOCKET>> taskQue_; // ��������� ��Ϣ : socket��

        CallBack callBack_{ nullptr }; // �ص�
        SOCKET socketServiceTcp_{ INVALID_SOCKET };
        Info info_; // �����ļ���Ϣ
        DataBase dataBaseInfo_;
    };
}

#endif //!__MY_NETWORKHANDLE_H__