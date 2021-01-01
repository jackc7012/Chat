#include "CNetWorkHandle.h"
using namespace cwy;

CNetWorkHandle::CNetWorkHandle()
{
    logNetWork.InitLog("../{time}/network");
    dataBase = CDataBase::CreateInstance();
    dataBase->InitDataBase("mychat");
}

CNetWorkHandle* CNetWorkHandle::CreateInstance()
{
    static CNetWorkHandle* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = new CNetWorkHandle();
    }
    return ptr;
}

CNetWorkHandle::~CNetWorkHandle()
{
    if (dataBase != nullptr) {
        delete dataBase;
    }
}

CommunicationType CNetWorkHandle::HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend)
{
    CommunicationType result = CommunicationType::NULLCOMMUNICATION;
    logNetWork << "recv " << (handleRecv.connectionType == ConnectionType::TCP ? "TCP" : "UDP") <<
        " message from " << handleRecv.ip << " , message content = " << message;
    logNetWork.PrintlogInfo(FILE_FORMAT);
    DecodeJson(message, handleRecv);
    s_HandleRecv toSend;
    toSend.connectionType = handleRecv.connectionType;
    switch (handleRecv.type) {
    case CommunicationType::REGISTER: {
        result = CommunicationType::REGISTERBACK;
        long long id = dataBase->InsertRegister(handleRecv.param.Register.customer, handleRecv.param.Register.password, handleRecv.ip);
        toSend.param.RegisterBack.customer = handleRecv.param.Register.customer;
        toSend.param.RegisterBack.register_result = ((id == -1) ? "failed" : "succeed");
        toSend.param.RegisterBack.id = id;
    }
        break;

    case CommunicationType::LOGIN: {
        auto itor = socketToIpName.find(handleRecv.socket.socket_accept_tcp);
        result = CommunicationType::LOGINBACK;
        toSend.param.LoginBack.customer = handleRecv.param.Login.customer;
        toSend.param.LoginBack.login_result = "failed";
        char* password = new char[50];
        if (password == nullptr) {
            result = CommunicationType::ERRORCOMMUNICATION;
        }   
        else {
            memset(password, 0, 50);
            std::string ip;
            int IsLogin = -1;
            int nRet = dataBase->SearchDataBaseLogin(handleRecv.param.Login.customer, ip, password, IsLogin);
            if (nRet != 0) {
                toSend.param.LoginBack.description = "no such people";
            }
            else if (IsLogin == 1) {
                toSend.param.LoginBack.login_result = "isLogin";
                toSend.param.LoginBack.description = "people has already login";
            }
            else if (strcmp(password, handleRecv.param.Login.password) != 0) {
                toSend.param.LoginBack.description = "password error";
            }
            else if (strcmp(password, handleRecv.param.Login.password) == 0) {
                itor->second.second = handleRecv.param.Login.customer;
                toSend.param.LoginBack.description = ((ip == handleRecv.ip) ? "login succeed" : "ip does not match last time");
                toSend.param.LoginBack.login_result = "succeed";
                dataBase->UpdateLoginStatus(handleRecv.param.Login.customer, 1);
            }
            delete[]password;
        }
    }
        break;

    }
    toSend.type = result;
    strToSend = EncodeJson(result, toSend);
    DeleteMemory(handleRecv.type, handleRecv);
    return result;
}

void CNetWorkHandle::SetSocketIp(const SOCKET& socket, const std::string& ip)
{
    socketToIpName.insert(std::pair<SOCKET, IpName>(socket, IpName(ip, "")));
}