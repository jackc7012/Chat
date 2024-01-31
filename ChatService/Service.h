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
        * type: 0  ֪ͨ���û� -> ��ǰ�����û�
        *       1  ֪ͨ�����û� -> ���û�����
        *       -1 ֪ͨ�����û� -> ���û�����
        */
        std::string HandleShowLogin(const UINT64 id = -1, const std::string& customerName = "", const int type = 0);

        void HandleLogOut(const UINT64 id);

        /*
        * type: 0 ����
        *       1 ͼƬ/�ļ�
        */
        void HandleChat(HandleRecv& handleRecv, const bool isOnline, const int type);

        std::string HandleChangePassword(HandleRecv& handleRecv);

        void HandleFileTransfer(HandleRecv& handleRecv);

    private:
        CallBack callBack_{ nullptr };
        Info info_;
        INT64 maxRegistered_{ DEFAULT_REGISTER_ID }; // ��ǰע��id
        DataBase* dataBase_{ nullptr }; // ���ݱ���
        std::vector<std::thread> threadHandle_; // �߳� : ������Ϣ
        SOCKET socketServiceTcp_{ INVALID_SOCKET };
        SOCKADDR_IN addrServiceTcp_{ 0 };
        std::thread threadAcc_, threadTcp_, threadHeartBeat_; // �߳� : ����socket, ������Ϣѹ�����, ����
        std::unordered_map<SOCKET, std::string> socketAccept_; // ��������δ��¼��socket socket�� : ip
        std::mutex mutexHandle_, mutexPush_;
        std::unordered_map<UINT64, LoginPeople> loginCustomer_;  // ��¼�û� id : LoginPeople
        std::queue<std::pair<std::string, SOCKET>> taskQue_; // ��������� ��Ϣ : socket��
        bool exitFlag{ false }; // �����˳���־
        UINT32 loginCount_{ 0 }; // ��¼����
        Log log_;
    };
}

#endif // ! __SERVICE_H__