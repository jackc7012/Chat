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
    if (StartNetWork() != 0)
    {
        return -1;
    }
    if (InitDataBase() != 0)
    {
        return -1;
    }
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

int NetWorkHandle::GetInfo(const std::string& infoFileName)
{
    info_.GetInfoFromFile(infoFileName);

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

    return StartTcp();
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

int NetWorkHandle::StartTcp()
{
    socketServiceTcp_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketServiceTcp_ == INVALID_SOCKET)
    {
        return callBack_("socket创建失败", true);
    }
    else
    {
        SOCKADDR_IN addrServiceTcp{ 0 };
        addrServiceTcp.sin_family = AF_INET;
        addrServiceTcp.sin_port = htons(info_.GetNetWorkInfo().tcpPort_);
        addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
        if (::bind(socketServiceTcp_, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR)
        {
            return callBack_("socket创建失败", true);
        }
        if (::listen(socketServiceTcp_, SOMAXCONN) == SOCKET_ERROR)
        {
            return callBack_("socket创建失败", true);
        }
        int nNetTimeout = 100;
        ::setsockopt(socketServiceTcp_, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
        threadAcc_ = std::thread(&NetWorkHandle::HandleAcc, this);
        threadTcp_ = std::thread(&NetWorkHandle::HandleTcp, this);
        threadHeartBeat_ = std::thread(&NetWorkHandle::HandleHeartBeat, this);
    }

    return 0;
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
            socketAccept_.insert(std::make_pair(socket, std::string(inet_ntoa(addrClient.sin_addr))));
        }
        Sleep(1);
    }

    return;
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
        auto itor = socketAccept_.begin();
        for (; itor != socketAccept_.end(); ++itor)
        {
            ret = ::recv(itor->first, strRecv, DATA_LENGTH, 0);
            if (ret > 0)
            {
                break;
            }
        }
        if (ret > 0)
        {
            //std::lock_guard<std::mutex> lg(mutexHandle_);
            taskQue_.push(std::make_pair(strRecv, itor->first));
        }
        delete[]strRecv;
        strRecv = nullptr;
        Sleep(1);
    }

    return;
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

void NetWorkHandle::NetWorkEvent(const std::string& taskParam, const SOCKET socket)
{
    JudgeCallBack();
    s_HandleRecv handleRecv, handleSend;
    handleSend.socket_accept_ = socket;
    DecodeJson(taskParam, handleRecv);
    std::string result;
    std::mutex mutexLogin;
    std::unique_lock<std::mutex> ul(mutexLogin, std::defer_lock);
    std::string ip;
    auto itor = socketAccept_.find(socket);
    if (itor != socketAccept_.end())
    {
        ip = itor->second;
    }
    callBack_(std::string("recv from " + ip + " message : " + taskParam), false);
    switch (handleRecv.type_)
    {
    case CommunicationType::REGISTER:
    {
        result = HandleRegister(handleRecv, ip, handleSend);
        ::send(socket, result.c_str(), result.length(), 0);
        break;
    }

    case CommunicationType::LOGIN:
    {
        bool isLoginSucceed = false;
        std::string customerName;
        result = HandleLogin(handleRecv, ip, handleSend, isLoginSucceed, customerName);
        ::send(socket, result.c_str(), result.length(), 0);
        if (isLoginSucceed)
        {
            result = HandleShowLogin();
            ::send(socket, result.c_str(), result.length(), 0);
            result = HandleShowLogin(handleRecv.Param.login_.id, 1);
            auto itor = loginCustomer_.begin();
            for (; itor != loginCustomer_.end(); ++itor)
            {
                if (itor->second.second == socket)
                {
                    continue;
                }
                ::send(itor->second.second, result.c_str(), result.length(), 0);
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
        result = HandleShowLogin(handleRecv.Param.delCustomer_.id, -1);
        auto itor = loginCustomer_.begin();
        for (; itor != loginCustomer_.end(); ++itor)
        {
            if (itor->second.second == socket)
            {
                continue;
            }
            ::send(itor->second.second, result.c_str(), result.length(), 0);
        }
        ul.lock();
        --loginCount_;
        auto itor1 = loginCustomer_.find(handleRecv.Param.delCustomer_.id);
        if (itor1 != loginCustomer_.end())
        {
            closesocket(itor1->second.second);
            loginCustomer_.erase(handleRecv.Param.delCustomer_.id);
        }
        ul.unlock();
        break;
    }

    case CommunicationType::FORCEDELETE:
    {
        ul.lock();
        auto itor1 = loginCustomer_.find(handleRecv.Param.forceDelete_.id);
        if (itor1 != loginCustomer_.end())
        {
            ::send(itor1->second.second, taskParam.c_str(), taskParam.length(), 0);
            closesocket(itor1->second.second);
            std::string customerName = itor1->second.first;
            loginCustomer_.erase(handleRecv.Param.forceDelete_.id);
            loginCustomer_.insert(std::make_pair(handleRecv.Param.forceDelete_.id, std::make_pair(customerName, socket)));
        }
        ul.unlock();
        result = HandleShowLogin();
        ::send(socket, result.c_str(), result.length(), 0);
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

std::string NetWorkHandle::HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend)
{
    RegisterSpace(&handleSend.Param.registerBack_.customer, handleRecv.Param.register_.customer);
    if (strlen(handleRecv.Param.register_.password) > 64)
    {
        callBack_(handleSend.Param.registerBack_.customer + std::string(" register failed, failed description : password to long"), true);
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
        RegisterSpace(&handleSend.Param.registerBack_.description, "password to long");
    }
    else
    {
        SqlRequest sql("insert into tb_info(ID, Name, Password, IsLogin, LastLoginTime, RegisterIp) values");
        sql << dbJoin(std::vector<std::string>{std::to_string(maxRegistered_), handleRecv.Param.register_.customer,
            handleRecv.Param.register_.password, "0", GetSystemTime(), ip});
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
            RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
            RegisterSpace(&handleSend.Param.registerBack_.description, "unkown error");
        }
    }

    handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
        ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

std::string NetWorkHandle::HandleLogin(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend, 
    bool& isLoginSucceed, std::string& customerName)
{
    RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
    customerName = "";
    SqlRequest sql("select Name, Password, IsLogin, RecentIp from tb_info where ID = ");
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
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(1)) != handleRecv.Param.login_.password)
    {
        RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
    }
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(2)) == "1")
    {
        delete[]handleRecv.Param.loginBack_.login_result;
        RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo.at(0).at(0));
        RegisterSpace(&handleSend.Param.loginBack_.login_result, ip);
        RegisterSpace(&handleSend.Param.loginBack_.description,
            "this user has already login in, please make sure or modify your password");
    }
    else if (loginInfo.size() == 1)
    {
        RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo.at(0).at(0));
        RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
        sql.clear();
        sql << "update tb_info set IsLogin = 1, RecentIp = " << toDbString(ip) << " where ID = " << handleRecv.Param.login_.id;
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
