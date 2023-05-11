#include "NetWorkHandle.h"

using namespace cwy;

NetWorkHandle::NetWorkHandle()
{
    threadHandle_.clear();
    socketAccept_.clear();
}

NetWorkHandle::~NetWorkHandle()
{
    exitFlag = true;
    for (unsigned short i = 0; i < info_.GetCommonInfo().threadNum_; ++i)
    {
        if (threadHandle_[i].joinable())
        {
            threadHandle_[i].join();
        }
    }
    if (threadAcc_.joinable())
    {
        threadAcc_.join();
    }
    if (threadTcp_.joinable())
    {
        threadTcp_.join();
    }
    if (threadHeartBeat_.joinable())
    {
        threadHeartBeat_.join();
    }
}

int NetWorkHandle::Init(CallBack callBack, const std::string& infoFileName)
{
    SetEvent(callBack);
    GetInfo(infoFileName);
    InitThread();
    StartNetWork();
    InitDataBase();
    SqlRequest sql("select max(ID) from tb_info");
    DataRecords loginInfo;
    dataBase_->execSql(sql, &loginInfo);
    if (loginInfo.size() != 0)
    {
        std::stringstream ID;
        ID << loginInfo.at(0).at(0);
        ID >> maxRegistered_;
        if (maxRegistered_ < DEFAULT_REGISTER_ID)
        {
            maxRegistered_ = DEFAULT_REGISTER_ID;
        }
    }
    ++maxRegistered_;
    return 0;
}


int NetWorkHandle::SetEvent(CallBack callBack)
{
    if (callBack_ == nullptr)
    {
        callBack_ = callBack;
    }
    return 0;
}

int NetWorkHandle::InitThread()
{
    for (unsigned short i = 0; i < info_.GetCommonInfo().threadNum_; ++i)
    {
        threadHandle_.emplace_back(std::thread(&NetWorkHandle::ThreadHandler, this, i));
    }
    return 0;
}

int NetWorkHandle::StartNetWork()
{
    JudgeCallBack();
    char name[256] = { 0 };
    std::string result;
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    int getNameRet = gethostname(name, sizeof(name));
    if (getNameRet == 0)
    {
        hostent* host = gethostbyname(name);
        if (NULL == host)
        {
            return callBack_("本地ip获取失败，程序将退出！", true);
        }
        else
        {
            callBack_("本地监听ip：" + std::string(inet_ntoa(*(in_addr*)*host->h_addr_list)), false);
        }
    }
    else
    {
        return callBack_("本地ip获取失败，程序将退出！", true);
    }
    StartTcp();

    return 0;
}

int NetWorkHandle::InitDataBase()
{
    JudgeCallBack();
    cwy::DataBase dataBaseInfo = info_.GetDataBaseInfo();
    if (!dataBase_->initSqlDataBase(dataBaseInfo.ip_, dataBaseInfo.name_, dataBaseInfo.uid_, dataBaseInfo.pwd_))
    {
        return callBack_("数据库连接失败！", true);
    }
    return 0;
}

std::string NetWorkHandle::HandleRegister(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend)
{
    RegisterSpace(&handleSend.Param.registerBack_.customer, handleRecv.Param.register_.customer);
    RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
    SqlRequest sql("insert into tb_info(ID, Name, Password, IsLogin, LastLoginTime, RegisterMAC, RegisterIp) values");
    sql << dbJoin(std::vector<std::string>{std::to_string(maxRegistered_), handleRecv.Param.register_.customer,
        Encryption(handleRecv.Param.register_.password), "0", GetSystemTime(), handleRecv.Param.register_.mac,
        handleRecv.Param.register_.ip});
    if (dataBase_->execSql(sql) == TRUE)
    {
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");
        handleSend.Param.registerBack_.id = maxRegistered_;
        sql.clear();
        sql << "create table [" << maxRegistered_ << "_Src"
            << "] ( TargetId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL)";
        if (dataBase_->execSql(sql) == FALSE)
        {
            callBack_(handleSend.Param.registerBack_.customer + std::string(" create source table failed, failed description : ")
                + dataBase_->getErrMessage(), true);
        }
        sql.clear();
        sql << "create table [" << maxRegistered_ << "_Trg"
            << "] ( SourceId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL, IsRead bit NOT NULL)";
        if (dataBase_->execSql(sql) == FALSE)
        {
            callBack_(handleSend.Param.registerBack_.customer + std::string(" create target table failed, failed description : ")
                + dataBase_->getErrMessage(), true);
        }
        ++maxRegistered_;
    }
    else
    {
        callBack_(handleSend.Param.registerBack_.customer + std::string(" register failed, failed description : ")
            + dataBase_->getErrMessage(), true);
        RegisterSpace(&handleSend.Param.registerBack_.description, "unkown error");
    }

    handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
        ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

std::string NetWorkHandle::HandleLogin(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend, bool& isLoginSucceed,
    std::string& customerName)
{
    RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
    customerName = "";
    SqlRequest sql("select Name, Password, IsLogin from tb_info where ID = ");
    sql << handleRecv.Param.login_.id;
    DataRecords loginInfo;
    bool ret = dataBase_->execSql(sql, &loginInfo);
    if (!ret)
    {
        callBack_(std::to_string(handleRecv.Param.login_.id) + " select login fail, description : " + dataBase_->getErrMessage(), true);
    }
    else if (loginInfo.size() == 0)
    {
        RegisterSpace(&handleSend.Param.loginBack_.description, "there is no such user");
    }
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(2)) == "1")
    {
        RegisterSpace(&handleSend.Param.loginBack_.description,
            "this user has already login in, please make sure or modify your password");
    }
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(1)) != handleRecv.Param.login_.password)
    {
        RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
    }
    else if (loginInfo.size() == 1)
    {
        RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo.at(0).at(0));
        RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
        sql.clear();
        sql << "update tb_info set IsLogin = 1 where ID = " << handleRecv.Param.login_.id;
        if (dataBase_->execSql(sql) == FALSE)
        {
            callBack_(std::to_string(handleRecv.Param.login_.id) + " update login fail, description : " + dataBase_->getErrMessage(),
                true);
        }
        customerName = handleSend.Param.loginBack_.customer;
        isLoginSucceed = true;
    }
    else
    {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.loginBack_.login_result, "succeed") == 0)
        ? CommunicationType::LOGINBACKSUCCEED : CommunicationType::LOGINBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

void NetWorkHandle::HandleExit(const UINT64 id)
{
    SqlRequest sql;
    sql << "update tb_info set IsLogin = 0, LastLoginTime = " << toDbString(GetSystemTime())
        << " where ID = " << toDbString(std::to_string(id));
    if (dataBase_->execSql(sql) == FALSE)
    {
        callBack_("login out " + std::to_string(id) + " failed description : " + dataBase_->getErrMessage(), true);
    }
}

std::string NetWorkHandle::HandleChat(const s_HandleRecv& handleRecv, const bool isOnline, s_HandleRecv& handleSend)
{
    handleSend.type_ = CommunicationType::CHAT;
    // source table
    std::string sourceTableName(std::to_string(handleRecv.Param.chat_.sourceid) + "_Src");
    SqlRequest sql("insert into [");
    sql << sourceTableName << "] (TargetId, Time, ChatContent) values" << dbJoin({ std::to_string(handleRecv.Param.chat_.targetid),
        GetSystemTime(), handleRecv.Param.chat_.content });
    if (dataBase_->execSql(sql) == FALSE)
    {
        callBack_(std::to_string(handleSend.Param.chat_.sourceid) + " insert source table [" + sourceTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }
    // target table
    std::string targetTableName(std::to_string(handleRecv.Param.chat_.targetid) + "_Trg");
    sql.clear();
    sql << "insert into [" << targetTableName << "] (SourceId, Time, ChatContent, IsRead) values"
        << dbJoin({ std::to_string(handleRecv.Param.chat_.sourceid), GetSystemTime(), handleRecv.Param.chat_.content,
            (isOnline ? "1" : "0") });
    if (dataBase_->execSql(sql) == FALSE)
    {
        callBack_(std::to_string(handleSend.Param.chat_.sourceid) + " insert target table [" + targetTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }

    if (isOnline)
    {
        RegisterSpace(&handleSend.Param.chat_.content, handleRecv.Param.chat_.content);
        handleSend.Param.chat_.sourceid = handleRecv.Param.chat_.sourceid;
        handleSend.Param.chat_.targetid = handleRecv.Param.chat_.targetid;
        std::string result = EncodeJson(handleSend.type_, handleSend);
        UnregisterSpace(handleSend.type_, handleSend);
        return result;
    }
    return "";
}

std::string NetWorkHandle::HandleShowLogin(const UINT64 id/* = -1*/, const int type/* = 0*/)
{
    std::string loginInfos;
    s_HandleRecv handleSend;
    handleSend.type_ = CommunicationType::SHOWLOGIN;
    handleSend.Param.showLogin_.show_login_type = type;
    if (type == -1 || type == 1)
    {
        auto itor = loginCustomer_.find(id);
        if (itor == loginCustomer_.end())
        {
            return "";
        }
        loginInfos = CombineString({ {std::to_string(id), itor->second.first, ((type == -1) ? "0" : "1")} });
    }
    else
    {
        SqlRequest sql("select ID, Name, IsLogin from tb_info");
        DataRecords loginInfo;
        bool ret = dataBase_->execSql(sql, &loginInfo);
        if (!ret)
        {
            callBack_(std::to_string(id) + " select show login fail, description : " + dataBase_->getErrMessage(), true);
        }
        loginInfos = CombineString(loginInfo);
    }
    RegisterSpace(&handleSend.Param.showLogin_.customer, loginInfos);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

int NetWorkHandle::StartTcp()
{
    socketServiceTcp_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketServiceTcp_ == INVALID_SOCKET)
    {
        callBack_("", true);
    }
    else
    {
        addrServiceTcp_.sin_family = AF_INET;
        addrServiceTcp_.sin_port = htons(info_.GetNetWorkInfo().tcpPort_);
        addrServiceTcp_.sin_addr.S_un.S_addr = INADDR_ANY;
        if (::bind(socketServiceTcp_, (sockaddr*)&addrServiceTcp_, sizeof(addrServiceTcp_)) == SOCKET_ERROR)
        {
            callBack_("", true);
        }
        if (::listen(socketServiceTcp_, SOMAXCONN) == SOCKET_ERROR)
        {
            callBack_("", true);
        }
        threadAcc_ = std::thread(&NetWorkHandle::HandleAcc, this);
        threadTcp_ = std::thread(&NetWorkHandle::HandleTcp, this);
        threadHeartBeat_ = std::thread(&NetWorkHandle::HandleHeartBeat, this);
    }
    return 0;
}

int NetWorkHandle::GetInfo(const std::string& infoFileName)
{
    info_.GetInfoFromFile(infoFileName);
    return 0;
}

void NetWorkHandle::ThreadHandler(const unsigned short threadNum)
{
    while (true)
    {
        if (exitFlag)
        {
            break;
        }
        std::string taskParam;
        SOCKET socket;
        {
            std::lock_guard<std::mutex> lg(mutexHandle_);
            if (taskQue_.empty())
            {
                Sleep(1);
                continue;
            }
            std::pair<std::string, SOCKET> param = taskQue_.front();
            taskQue_.pop();
            taskParam = param.first;
            socket = param.second;
        }
        NetWorkEvent(taskParam, socket);
        Sleep(1);
    }
    return;
}

void NetWorkHandle::HandleAcc()
{
    while (true)
    {
        if (exitFlag)
        {
            break;
        }
        SOCKADDR_IN addrClient = { 0 };
        int len = sizeof(SOCKADDR);
        SOCKET socket = ::accept(socketServiceTcp_, (SOCKADDR*)&addrClient, &len);
        if (socket == SOCKET_ERROR)
        {
            Sleep(1);
            continue;
        }
        {
            std::lock_guard<std::mutex> lg(mutexPush_);
            socketAccept_.push_back(socket);
        }
        Sleep(1);
    }
}

void NetWorkHandle::HandleTcp()
{
    while (true)
    {
        if (exitFlag)
        {
            break;
        }
        char* strRecv = new char[DATA_LENGTH];
        memset(strRecv, 0, DATA_LENGTH);
        int ret = 0;
        unsigned int i = 0;
        for (; i < socketAccept_.size(); ++i)
        {
            ret = ::recv(socketAccept_[i], strRecv, DATA_LENGTH, 0);
            if (ret > 0)
                break;
        }
        if (ret > 0)
        {
            std::lock_guard<std::mutex> lg(mutexHandle_);
            taskQue_.push(std::make_pair(strRecv, socketAccept_[i]));
        }
        delete[]strRecv;
        strRecv = nullptr;
        Sleep(1);
    }
}

void NetWorkHandle::HandleHeartBeat()
{
    while (true)
    {
        if (exitFlag)
        {
            break;
        }
        Sleep(1);
    }
}

void NetWorkHandle::NetWorkEvent(const std::string& taskParam, const SOCKET socket)
{
    s_HandleRecv handleRecv, handleSend;
    handleSend.socket_accept_ = socket;
    DecodeJson(taskParam, handleRecv);
    std::string result;
    std::mutex mutexLogin;
    std::unique_lock<std::mutex> ul(mutexLogin, std::defer_lock);
    switch (handleRecv.type_)
    {
    case CommunicationType::REGISTER:
    {
        result = HandleRegister(handleRecv, handleSend);
        ::send(socket, result.c_str(), result.length(), 0);
        break;
    }

    case CommunicationType::LOGIN:
    {
        bool isLoginSucceed = false;
        std::string customerName;
        result = HandleLogin(handleRecv, handleSend, isLoginSucceed, customerName);
        ::send(socket, result.c_str(), result.length(), 0);
        result = HandleShowLogin();
        ::send(socket, result.c_str(), result.length(), 0);
        result = HandleShowLogin(handleRecv.Param.login_.id, 1);
        if (isLoginSucceed)
        {
            for (size_t i = 0; i < socketAccept_.size(); ++i)
            {
                if (socketAccept_.at(i) == socket)
                {
                    continue;
                }
                ::send(socketAccept_.at(i), result.c_str(), result.length(), 0);
            }
            ul.lock();
            ++loginCount_;
            loginCustomer_.insert(std::make_pair(handleRecv.Param.login_.id, std::make_pair(customerName, socket)));
            ul.unlock();
        }
        break;
    }

    case CommunicationType::DELETECUSTOMER:
    {
        HandleExit(handleRecv.Param.delCustomer_.id);
        std::string strLogin = HandleShowLogin(handleRecv.Param.delCustomer_.id, -1);
        for (size_t i = 0; i < socketAccept_.size(); ++i)
        {
            if (socketAccept_.at(i) == socket)
            {
                continue;
            }
            ::send(socketAccept_.at(i), strLogin.c_str(), strLogin.length(), 0);
        }
        ul.lock();
        --loginCount_;
        auto itor = loginCustomer_.find(handleRecv.Param.delCustomer_.id);
        if (itor != loginCustomer_.end())
        {
            closesocket(itor->second.second);
            loginCustomer_.erase(handleRecv.Param.delCustomer_.id);
        }
        ul.unlock();
        break;
    }

    case CommunicationType::CHAT:
    {
        bool isOnline = true;
        auto itor = loginCustomer_.find(handleRecv.Param.chat_.targetid);
        if (itor == loginCustomer_.end())
        {
            isOnline = false;
        }
        result = HandleChat(handleRecv, isOnline, handleSend);
        if (itor != loginCustomer_.end())
        {
            ::send(itor->second.second, result.c_str(), result.length(), 0);
        }
        break;
    }

    default:
        break;
    }
    UnregisterSpace(handleRecv.type_, handleRecv);
    return;
}
