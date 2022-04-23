#include "CNetWorkHandle.h"

#include <sstream>
using namespace cwy;

CNetWorkHandle::CNetWorkHandle()
: mainWnd_(nullptr)
, udpSocket_(0)
{
    logNetWork_.InitLog("../{time}/network");
    dataBase_ = CDataBase::CreateInstance();
    dataBase_->InitDataBase(DATABASE_NAME);
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
    isExit_ = true;
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        if (myHandleThread_[i].joinable()) {
            myHandleThread_[i].join();
        }
    }
    dataBase_->UpdateLoginStatus();
    if (dataBase_ != nullptr) {
        delete dataBase_;
    }
}

std::vector<std::string> CNetWorkHandle::InitNetWork(const HWND hWnd)
{
    for (unsigned short i = 0; i < THREAD_NUM; ++i) {
        myHandleThread_[i] = std::thread(&CNetWorkHandle::threadTask, this, i);
    }
    mainWnd_ = hWnd;
    std::vector<std::string> result;
    char name[256];
    int getNameRet = gethostname(name, sizeof(name));
    if (getNameRet == 0) {
        hostent* host = gethostbyname(name);
        if (NULL == host) {
            return std::vector<std::string>{};
        }
        in_addr* pAddr = (in_addr*)*host->h_addr_list;
        for (int i = 0; i < (strlen((char*)*host->h_addr_list) - strlen(host->h_name)) / 4 && pAddr; i++) {
            std::string addr = inet_ntoa(pAddr[i]);
            result.push_back(addr);
        }
    } else {
        return std::vector<std::string>{};
    }

    return result;
}

void CNetWorkHandle::SetUdpSocket(SOCKET udpSocket)
{
    udpSocket_ = udpSocket;
}

void CNetWorkHandle::threadTask(unsigned short taskNum)
{
    s_HandleRecv temp;
    std::string message;
    CommunicationType type = CommunicationType::NULLCOMMUNICATION;
    std::string result_message;
    while (1) {
        if (isExit_) {
            break;
        }
        if (!taskQueue_.empty()) {
            {
                std::lock_guard<std::mutex> lg(mtServerHandle_);
                if (!taskQueue_.empty()) {
                    temp.connectionType_ = taskQueue_.front().handleRecv_.connectionType_;
                    temp.ip_ = taskQueue_.front().handleRecv_.ip_;
                    temp.socket_ = taskQueue_.front().handleRecv_.socket_;
                    message = taskQueue_.front().message_;
                    taskQueue_.pop();
                }
            }
            if (message == "") {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            type = HandleRecv(message, temp, result_message);
            if (type == CommunicationType::NULLCOMMUNICATION) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            if (temp.connectionType_ == ConnectionType::TCP) {
                ::send(temp.socket_.socketAccept_, result_message.c_str(), result_message.length(), 0);
            }
            else if (temp.connectionType_ == ConnectionType::UDP) {
                int addrLen = sizeof(SOCKADDR);
                auto itor = idToSockaddrinUdp_.find(temp.param_.chat_.target_);
                int ret = ::sendto(udpSocket_, result_message.c_str(), result_message.length(), 0,
                    (SOCKADDR*)&(itor->second), addrLen);
                int c = GetLastError();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool CNetWorkHandle::ClientAccept(const SOCKET& socket, const SOCKADDR_IN& sockAddr)
{
    int addr_len = sizeof(sockAddr);
    SOCKET addrClient = ::accept(socket, (SOCKADDR*)&sockAddr, &addr_len);
    if (addrClient == INVALID_SOCKET) {
        return false;
    }
    std::string ip = ::inet_ntoa(sockAddr.sin_addr);
    SetSocketInfo(addrClient, ip);
    logNetWork_ << "ip = " << ip << " connect";
    logNetWork_.PrintlogInfo(FILE_FORMAT);

    return true;
}

void CNetWorkHandle::HandleRecvTcp()
{
    char* strRecv = new char[DATA_LENGTH];
    memset(strRecv, 0, DATA_LENGTH);
    int i = socketAccept_.size() - 1;
    for (; i >= 0; --i) {
        int nRet = ::recv(socketAccept_[i], strRecv, DATA_LENGTH, 0);
        if (nRet > 0)
            break;
    }
    s_HandleRecv handleRecv;
    handleRecv.connectionType_ = ConnectionType::TCP;
    handleRecv.socket_.socketAccept_ = socketAccept_[i];
    auto itor = socketToClientInfoTcp_.find(socketAccept_[i]);
    handleRecv.ip_ = itor->second.ip_;
    taskQueue_.push(s_TaskQueue(handleRecv, strRecv));
    delete[]strRecv;
}

void CNetWorkHandle::HandleRecvUdp()
{
    char* strRecv = new char[DATA_LENGTH];
    memset(strRecv, 0, DATA_LENGTH);
    SOCKADDR_IN addrClient = { 0 };
    int addrLen = sizeof(SOCKADDR);
    int nRet = ::recvfrom(udpSocket_, strRecv, DATA_LENGTH, 0, (SOCKADDR*)&addrClient, &addrLen);
    if (std::string(strRecv).substr(0, 6) == "Accept") {
        idToSockaddrinUdp_[atoll(std::string(strRecv).substr(7, 5).c_str())] = addrClient;
    } else {
        s_HandleRecv handleRecv;
        handleRecv.connectionType_ = ConnectionType::UDP;
        handleRecv.ip_ = ::inet_ntoa(addrClient.sin_addr);
        taskQueue_.push(s_TaskQueue(handleRecv, strRecv));
    }
    delete[]strRecv;
}

CommunicationType CNetWorkHandle::HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend)
{
    CommunicationType result = CommunicationType::NULLCOMMUNICATION;
    logNetWork_ << "recv " << (handleRecv.connectionType_ == ConnectionType::TCP ? "TCP" : "UDP") <<
        " message from " << handleRecv.ip_ << " , message content = " << message;
    logNetWork_.PrintlogInfo(FILE_FORMAT);
    DecodeJson(message, handleRecv);
    s_HandleRecv toSend;
    toSend.connectionType_ = handleRecv.connectionType_;
    switch (handleRecv.type_) {
    case CommunicationType::TOKENBACK: {
        auto itor = socketToClientInfoTcp_.find(handleRecv.socket_.socketAccept_);
        if (itor->second.token_ != handleRecv.param_.tokenBack_.content_) {
            itor->second.token_ = "";
        }
    }
    break;

    case CommunicationType::REGISTER: {
        result = CommunicationType::REGISTERBACK;
        long long id = dataBase_->InsertRegister(handleRecv.param_.register_.customer_, handleRecv.param_.register_.password_, handleRecv.ip_);
        toSend.param_.registerBack_.customer_ = handleRecv.param_.register_.customer_;
        toSend.param_.registerBack_.registerResult_ = ((id == -1) ? "failed" : "succeed");
        toSend.param_.registerBack_.id_ = id;
    }
    break;

    case CommunicationType::LOGIN: {
        result = CommunicationType::LOGINBACK;
        toSend.param_.loginBack_.customer_ = "";
        toSend.param_.loginBack_.loginResult_ = "failed";
        char* password = new char[50];
        if (password == nullptr) {
            result = CommunicationType::ERRORCOMMUNICATION;
        } else {
            memset(password, 0, 50);
            std::string name;
            std::string ip;
            int IsLogin = -1;
            int nRet = dataBase_->SearchDataBaseLogin(handleRecv.param_.login_.id_, name, ip, password, IsLogin);
            toSend.param_.loginBack_.customer_ = const_cast<char*>(name.c_str());
            if (nRet != 0) {
                toSend.param_.loginBack_.description_ = "no such people";
            } else if (IsLogin == 1) {
                toSend.param_.loginBack_.loginResult_ = "isLogin";
                toSend.param_.loginBack_.description_ = "people has already login";
            } else if (strcmp(password, handleRecv.param_.login_.password_) != 0) {
                toSend.param_.loginBack_.description_ = "password error";
            } else if (strcmp(password, handleRecv.param_.login_.password_) == 0) {
                auto itor = socketToClientInfoTcp_.find(handleRecv.socket_.socketAccept_);
                itor->second.name_ = name;
                itor->second.id_ = handleRecv.param_.login_.id_;
                toSend.param_.loginBack_.description_ = ((ip == handleRecv.ip_) ? "login succeed" : "ip does not match last time");
                toSend.param_.loginBack_.loginResult_ = "succeed";
                dataBase_->UpdateLoginStatus(1, handleRecv.param_.login_.id_);
                std::ostringstream tmp;
                tmp << handleRecv.param_.login_.id_ << "\t\t" << name;
                std::string *toName = new std::string(tmp.str());
                ::PostMessage(mainWnd_, WM_TO_MAIN_MESSAGE, (WPARAM)CommunicationType::LOGIN, (LPARAM)toName);
            }
            delete[]password;
        }
    }
    break;

    case CommunicationType::CHAT: {
        result = CommunicationType::CHATRECV;
        toSend.param_.chatRecv_.content_ = handleRecv.param_.chat_.content_;
    }
    break;

    }
    toSend.type_ = result;
    strToSend = EncodeJson(result, toSend);
    DeleteMemory(handleRecv.type_, handleRecv);

    return result;
}

void CNetWorkHandle::SetSocketInfo(const SOCKET& socket, const std::string& ip)
{
    std::string token;
    int tokenLength = 0;
    GetToken(token, tokenLength);
    socketAccept_.push_back(socket);
    socketToClientInfoTcp_.insert(std::pair<SOCKET, ClientInfoTcp>(socket, ClientInfoTcp(ip, "", token)));
    s_HandleRecv toSend;
    toSend.param_.token_.tokenLength_ = tokenLength;
    toSend.param_.token_.content_ = const_cast<char *>(token.c_str());
    std::string strToSend = EncodeJson(CommunicationType::TOKEN, toSend);
    ::send(socket, strToSend.c_str(), strToSend.length(), 0);
}