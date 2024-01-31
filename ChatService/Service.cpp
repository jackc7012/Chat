#include "service.h"

using namespace cwy;

Service::Service()
{
}

Service::~Service()
{
    StopTcp();
}

int Service::Init(CallBack callBack)
{
    int ret = -1;
    do
    {
        if (callBack == nullptr)
        {
            break;
        }
        SetEvent(callBack);
        GetInfo();
        InitLog();
        if (InitDataBase() != 0)
        {
            break;
        }
        std::string sql("select max(ID) from tb_info");
        DataRecords loginInfo;
        if (!dataBase_->selectSql(sql, loginInfo))
        {
            break;
        }
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
        ret = 0;
    } while (0);

    return ret;
}

std::string Service::GetServerIp()
{
    std::string ip = "";
    do
    {
        char name[256] = { 0 };
        int getNameRet = gethostname(name, sizeof(name));
        if (getNameRet == 0)
        {
            hostent* host = gethostbyname(name);
            if (NULL != host)
            {
                ip = std::string(inet_ntoa(*(in_addr*)*host->h_addr_list));
            }
        }
    } while (0);

    return ip;
}

SOCKET Service::StartTcp()
{
    do
    {
        socketServiceTcp_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socketServiceTcp_ == INVALID_SOCKET)
        {
            callBack_(CallBackType::INIT, "socket创建失败", true);
            break;
        }
        else
        {
            SOCKADDR_IN addrServiceTcp{ 0 };
            addrServiceTcp.sin_family = AF_INET;
            addrServiceTcp.sin_port = htons(info_.GetNetWorkInfo().tcpPort_);
            addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
            if (::bind(socketServiceTcp_, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR)
            {
                ::closesocket(socketServiceTcp_);
                socketServiceTcp_ = INVALID_SOCKET;
                callBack_(CallBackType::INIT, "socket创建失败", true);
                break;
            }
            if (::listen(socketServiceTcp_, SOMAXCONN) == SOCKET_ERROR)
            {
                ::closesocket(socketServiceTcp_);
                socketServiceTcp_ = INVALID_SOCKET;
                callBack_(CallBackType::INIT, "socket创建失败", true);
                break;
            }
            int nNetTimeout = 100;
            ::setsockopt(socketServiceTcp_, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
            InitThread();
            exitFlag = false;
            //threadHeartBeat_ = std::thread(&Service::HandleHeartBeat, this);
        }
    } while (0);

    return socketServiceTcp_;
}

void Service::StopTcp()
{
    exitFlag = true;
    for (unsigned short i = 0; i < threadHandle_.size(); ++i)
    {
        if (threadHandle_[i].joinable())
        {
            threadHandle_[i].join();
        }
    }
    threadHandle_.clear();
    if (threadHeartBeat_.joinable())
    {
        threadHeartBeat_.join();
    }
    ::closesocket(socketServiceTcp_);
}

SOCKET Service::ClientAccept()
{
    SOCKET socketClient;
    do
    {
        SOCKADDR_IN sockAddr{ 0 };
        int addr_len = sizeof(sockAddr);
        socketClient = ::accept(socketServiceTcp_, (SOCKADDR*)&sockAddr, &addr_len);
        if (socketClient == INVALID_SOCKET)
        {
            callBack_(CallBackType::ACCEPT, "socket接收失败", true);
            break;
        }
        std::string ip = ::inet_ntoa(sockAddr.sin_addr);
        socketAccept_.insert(std::pair<SOCKET, std::string>(socketClient, ip));
    } while (0);

    return socketClient;
}

int Service::ClientRecv(const SOCKET& socketClient)
{
    int ret = -1;
    do
    {
        if (socketAccept_.find(socketClient) == socketAccept_.end())
        {
            callBack_(CallBackType::RECV, "socket传参错误" + std::to_string(socketClient), true);
            break;
        }
        char* strRecv = new char[DATA_LENGTH];
        memset(strRecv, 0, DATA_LENGTH);
        int ret = ::recv(socketClient, strRecv, DATA_LENGTH, 0);
        if (ret > 0)
        {
            taskQue_.push(std::make_pair(strRecv, socketClient));
        }
        delete[]strRecv;
        strRecv = nullptr;
    } while (0);

    return ret;
}

void Service::SetEvent(CallBack callBack)
{
    if (callBack_ == nullptr)
    {
        callBack_ = callBack;
    }
}

void Service::GetInfo()
{
    info_.GetInfoFromFile(INFO_FILE_NAME);
}

void Service::InitLog()
{
    LogInfo logInfo = info_.GetLogInfo();
    log_.InitLog(0, logInfo.path_, static_cast<LogLevel>(logInfo.level_), logInfo.size_, logInfo.autoFlush_);
}

void Service::InitThread()
{
    unsigned int threadNum = info_.GetCommonInfo().threadNum_;
    for (unsigned short i = 0; i < threadNum; ++i)
    {
        threadHandle_.emplace_back(std::thread(&Service::ThreadHandler, this, i));
    }
}

int Service::InitDataBase()
{
    DataBaseInfo dataBaseInfo = info_.GetDataBaseInfo();
    dataBase_ = DataBase::createInstance();
    if (!dataBase_->initDataBase(dataBaseInfo.ip_, dataBaseInfo.name_, dataBaseInfo.uid_, dataBaseInfo.pwd_))
    {
        callBack_(CallBackType::INIT, "数据库连接失败！", true);
        return -1;
    }

    return 0;
}

void Service::ThreadHandler(const UINT16 threadNum)
{
    while (1)
    {
        if (exitFlag)
        {
            break;
        }
        std::string taskParam;
        std::pair<std::string, SOCKET> param;
        SOCKET socket;
        {
            std::lock_guard<std::mutex> lg(mutexPush_);
            if (taskQue_.empty())
            {
                Sleep(1);
                continue;
            }
            param = taskQue_.front();
            taskQue_.pop();
        }
        taskParam = param.first;
        socket = param.second;
        NetWorkEvent(threadNum, taskParam, socket);
        Sleep(1);
    }
}

void Service::HandleHeartBeat()
{
    while (1)
    {

        Sleep(1);
    }
}

void Service::NetWorkEvent(const UINT16 threadNum, const std::string& taskParam, const SOCKET socket)
{
    auto itor = socketAccept_.find(socket);
    if (itor == socketAccept_.end())
    {
        return;
    }

    std::string ip = itor->second;
    HandleRecv handleRecv(socket, taskParam);
    std::string result;
    std::unique_lock<std::mutex> ul(mutexHandle_, std::defer_lock);
    callBack_(CallBackType::RECV, std::string("threadId : " + std::to_string(threadNum) + " : recv from " + ip +
        " message : " + taskParam), false);
    switch (handleRecv.GetType())
    {
        case CommunicationType::REGISTER:
        {
            result = HandleRegister(handleRecv, ip);
            ::send(socket, result.c_str(), result.length(), 0);
            break;
        }

        case CommunicationType::LOGIN:
        {
            bool isLoginSucceed = false;
            std::string customerName;
            result = HandleLogin(handleRecv, ip, isLoginSucceed, customerName);
            ::send(socket, result.c_str(), result.length(), 0);
            if (isLoginSucceed)
            {
                UINT64 id = strtoull(handleRecv.GetContent("id").c_str(), nullptr, 10);
                // 通知自己
                result = HandleShowLogin();
                ::send(socket, result.c_str(), result.length(), 0);
                // 通知别人
                result = HandleShowLogin(id, customerName, 1);
                auto itorEnd = loginCustomer_.end();
                for (auto itor = loginCustomer_.begin(); itor != itorEnd; ++itor)
                {
                    if (itor->second.socket_ == socket)
                    {
                        continue;
                    }
                    ::send(itor->second.socket_, result.c_str(), result.length(), 0);
                }
                ul.lock();
                ++loginCount_;
                loginCustomer_.insert(std::make_pair(id, LoginPeople(socket, customerName, ip)));
                ul.unlock();
                callBack_(CallBackType::LOGIN, handleRecv.GetContent("id") + COMBINE_ONE_CUSTOMER + customerName +
                    COMBINE_ONE_CUSTOMER + ip, false);
            }
            break;
        }

        case CommunicationType::DELCUSTOMER:
        {
            UINT64 id = strtoull(handleRecv.GetContent("id").c_str(), nullptr, 10);
            HandleLogOut(id);
            result = HandleShowLogin(id, "", -1);
            auto itorEnd = loginCustomer_.end();
            for (auto itor = loginCustomer_.begin(); itor != itorEnd; ++itor)
            {
                if (itor->second.socket_ == socket)
                {
                    continue;
                }
                ::send(itor->second.socket_, result.c_str(), result.length(), 0);
            }
            ul.lock();
            --loginCount_;
            auto itor = loginCustomer_.find(id);
            if (itor != loginCustomer_.end())
            {
                closesocket(itor->second.socket_);
                loginCustomer_.erase(id);
            }
            ul.unlock();
            callBack_(CallBackType::LOGOUT, handleRecv.GetContent("id"), false);
            break;
        }

        case CommunicationType::CHAT:
        case CommunicationType::TRANSFERFILEINFO:
        {
            bool isOnline = true;
            UINT64 targetId = strtoull(handleRecv.GetContent("targetid").c_str(), nullptr, 10);
            auto itor = loginCustomer_.find(targetId);
            if (itor == loginCustomer_.end())
            {
                isOnline = false;
            }
            HandleChat(handleRecv, isOnline, handleRecv.GetType() == CommunicationType::CHAT ? 0 : 1);
            if (isOnline)
            {
                ::send(itor->second.socket_, taskParam.c_str(), taskParam.length(), 0);
            }
            break;
        }

        case CommunicationType::CHANGEPASSWORD:
        {
            result = HandleChangePassword(handleRecv);
            ::send(socket, result.c_str(), result.length(), 0);
            break;
        }

        case CommunicationType::TRANSFERFILECONTENT:
        {

            break;
        }

        default:
            break;
    }
    return;
}

void Service::HandleKick(const UINT64 id)
{
    HandleLogOut(id);
    std::string result = HandleShowLogin(id, "", -1);
    auto itorEnd = loginCustomer_.end();
    for (auto itor = loginCustomer_.begin(); itor != itorEnd; ++itor)
    {
        if (itor->first == id)
        {
            continue;
        }
        ::send(itor->second.socket_, result.c_str(), result.length(), 0);
    }
    --loginCount_;
    auto itor = loginCustomer_.find(id);
    if (itor != loginCustomer_.end())
    {
        closesocket(itor->second.socket_);
        loginCustomer_.erase(id);
    }
}

std::string Service::HandleRegister(HandleRecv& handleRecv, const std::string& ip)
{
    bool isSucceed = false;
    HandleRecv handleSend;
    handleSend.SetContent("customer", handleRecv.GetContent("customer"));
    if (handleRecv.GetContent("password").length() <= 20)
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
            std::string(" register failed, failed description : password too short"), true);
        handleSend.SetContent("description", "password too short");
    }
    else
    {
        std::string registerId = std::to_string(maxRegistered_);
        SqlRequest sql("insert into tb_info(ID, Name, Password, IsLogin, LastLoginTime, RegisterIp) values");
        sql << dbJoin(std::vector<std::string>{registerId, handleRecv.GetContent("customer"),
            handleRecv.GetContent("password"), "0", GetSystemTime(), ip});
        if (sql.getErr() == ERRTYPE::COMMANDINJECTION)
        {
            callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
                std::string(" register failed, failed description : sql error : ") + sql.str(), true);
            handleSend.SetContent("description", "sql error");
        }
        else if (dataBase_->operSql(sql.str()))
        {
            handleSend.SetContent("id", registerId);
            sql.clear();
            sql << "create table [" << maxRegistered_ << "_Src"
                << "] ( TargetId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL, Type bit NOT NULL, index ["
                << maxRegistered_ << "_Src_Index](TargetId))";
            if (!dataBase_->operSql(sql.str()))
            {
                callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
                    std::string(" create source table failed, failed description : ") + dataBase_->getErrMessage(), true);
            }
            sql.clear();
            sql << "create table [" << maxRegistered_ << "_Trg"
                << "] ( SourceId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL, IsRead bit NOT NULL, Type bit NOT NULL, index ["
                << maxRegistered_ << "_Trg_Index](SourceId))";
            if (!dataBase_->operSql(sql.str()))
            {
                callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
                    std::string(" create target table failed, failed description : ") + dataBase_->getErrMessage(), true);
            }
            callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
                std::string(" register succeed, id = ") + registerId, false);
            ++maxRegistered_;
            isSucceed = true;
        }
        else
        {
            callBack_(CallBackType::SEND, handleRecv.GetContent("customer") +
                std::string(" register failed, failed description : ") + dataBase_->getErrMessage(), true);
            handleSend.SetContent("description", "unknown error");
        }
    }
    std::string result = handleSend.Write(isSucceed ? CommunicationType::REGISTERBACKSUCCEED :
        CommunicationType::REGISTERBACKFAILED);

    return result;
}

std::string Service::HandleLogin(HandleRecv& handleRecv, const std::string& ip, bool& isLoginSucceed, std::string& customerName)
{
    customerName = "";
    std::string result;
    HandleRecv handleSend;
    SqlRequest sql("select Name, Password, IsLogin, RecentIp from tb_info where ID = ");
    sql << handleRecv.GetContent("id");
    if (sql.getErr() == ERRTYPE::COMMANDINJECTION)
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("id") +
            std::string(" login failed, failed description : sql error : ") + sql.str(), true);
        handleSend.SetContent("description", "sql error");
        result = "";
    }
    else
    {
        DataRecords loginInfo;
        bool ret = dataBase_->selectSql(sql.str(), loginInfo);
        if (!ret)
        {
            callBack_(CallBackType::SEND, handleRecv.GetContent("id") + " login failed, failed description : " +
                dataBase_->getErrMessage(), true);
        }
        else if (loginInfo.size() == 0)
        {
            handleSend.SetContent("description", "there is no such user");
        }
        else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(1)) != handleRecv.GetContent("password"))
        {
            handleSend.SetContent("description", "password error");
        }
        else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(2)) == "1")
        {
            handleSend.SetContent("description", "this user has already login in, please make sure or modify your password");
        }
        else if (loginInfo.size() == 1)
        {
            handleSend.SetContent("customer", loginInfo.at(0).at(0));
            sql.clear();
            sql << "update tb_info set IsLogin = 1, RecentIp = " << toDbString(ip) << " where ID = " << handleRecv.GetContent("id");
            if (!dataBase_->operSql(sql.str()))
            {
                callBack_(CallBackType::SEND, handleRecv.GetContent("id") + " login failed, failed description : " +
                    dataBase_->getErrMessage(), true);
            }
            customerName = handleSend.GetContent("customer");
            isLoginSucceed = true;
        }
        else
        {
            handleSend.SetContent("description", "unkown error");
        }
        if (ret)
        {
            callBack_(CallBackType::SEND, isLoginSucceed ?
                (handleRecv.GetContent("id") + " login succeed ") :
                (handleRecv.GetContent("id") + " login failed, failed description : " + handleSend.GetContent("description")),
                !isLoginSucceed);
        }
        if (!isLoginSucceed)
        {
            handleSend.SetContent("id", handleRecv.GetContent("id"));
        }

        result = handleSend.Write(isLoginSucceed ? CommunicationType::LOGINBACKSUCCEED :
            CommunicationType::LOGINBACKFAILED);
    }

    return result;
}

std::string Service::HandleShowLogin(const UINT64 id/* = -1*/, const std::string& customerName/* = ""*/, const int type/* = 0*/)
{
    std::string loginInfos;
    HandleRecv handleSend;
    handleSend.SetContent("show_login_type", std::to_string(type));
    if (type == -1 || type == 1)
    { // 广播
        loginInfos = CombineString({ {std::to_string(id), customerName, ((type == 1) ? "1" : "-1")} });
    }
    else
    {
        SqlRequest sql("select ID, Name, IsLogin from tb_info");
        DataRecords loginInfo;
        bool ret = dataBase_->selectSql(sql.str(), loginInfo);
        if (!ret)
        {
            callBack_(CallBackType::SEND, std::to_string(id) + " show login failed, failed description : " +
                dataBase_->getErrMessage(), true);
        }
        loginInfos = CombineString(loginInfo);
    }
    handleSend.SetContent("customer", loginInfos);
    std::string result = handleSend.Write(CommunicationType::SHOWLOGIN);

    return result;
}

void Service::HandleLogOut(const UINT64 id)
{
    SqlRequest sql;
    sql << "update tb_info set IsLogin = 0, LastLoginTime = " << toDbString(GetSystemTime())
        << " where ID = " << toDbString(std::to_string(id));
    if (!dataBase_->operSql(sql.str()))
    {
        callBack_(CallBackType::SEND, std::to_string(id) + " log out failed, failed description : " +
            dataBase_->getErrMessage(), true);
    }
}

void Service::HandleChat(HandleRecv& handleRecv, const bool isOnline, const int type)
{
    // source table
    std::string sourceTableName(handleRecv.GetContent("sourceid") + "_Src");
    SqlRequest sql("insert into [");
    sql << sourceTableName << "] (TargetId, Time, ChatContent, Type) values" << dbJoin({ handleRecv.GetContent("targetid"),
        handleRecv.GetContent("time"), handleRecv.GetContent(type ? "file_info" : "content"), (type == 1 ? "1" : "0") });
    if (!dataBase_->operSql(sql.str()))
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("sourceid") + " insert source table [" + sourceTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }
    // target table
    std::string targetTableName(handleRecv.GetContent("targetid") + "_Trg");
    sql.clear();
    sql << "insert into [" << targetTableName << "] (SourceId, Time, ChatContent, IsRead, Type) values"
        << dbJoin({ handleRecv.GetContent("sourceid"), handleRecv.GetContent("time"), handleRecv.GetContent(type ? "file_info" : "content"),
            (isOnline ? "1" : "0"), (type == 1 ? "1" : "0") });
    if (!dataBase_->operSql(sql.str()))
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("sourceid") + " insert target table [" + targetTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }
}

std::string Service::HandleChangePassword(HandleRecv& handleRecv)
{
    SqlRequest sql("select Password from tb_info where ID = ");
    sql << toDbString(handleRecv.GetContent("id"));
    DataRecords info;
    HandleRecv handleSend;
    handleSend.SetContent("id", handleRecv.GetContent("id"));
    bool ret = dataBase_->selectSql(sql.str(), info);
    if (!ret)
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("id") + " changepassword failed, failed description : " +
            dataBase_->getErrMessage(), true);
        handleSend.SetContent("update_result", "failed");
        handleSend.SetContent("description", dataBase_->getErrMessage());
    }
    else if (info.size() == 1)
    {
        if (trim(info.at(0).at(0)) == handleRecv.GetContent("old_password"))
        {
            sql.clear();
            sql << "update tb_info set Password = " << toDbString(handleRecv.GetContent("password")) <<
                " where ID = " << toDbString(handleRecv.GetContent("id"));
            if (!dataBase_->operSql(sql.str()))
            {
                callBack_(CallBackType::SEND, handleRecv.GetContent("id") + " changepassword failed, failed description : " +
                    dataBase_->getErrMessage(), true);
                handleSend.SetContent("update_result", "failed");
                handleSend.SetContent("description", dataBase_->getErrMessage());
            }
            else
            {
                handleSend.SetContent("update_result", "succeed");
            }
        }
        else
        {
            callBack_(CallBackType::SEND, handleRecv.GetContent("id") +
                " changepassword failed, failed description : old_password error", true);
            handleSend.SetContent("update_result", "failed");
            handleSend.SetContent("description", "old_password error");
        }
    }
    else
    {
        callBack_(CallBackType::SEND, handleRecv.GetContent("id") +
            " changepassword failed, failed description : unknown error", true);
        handleSend.SetContent("update_result", "failed");
        handleSend.SetContent("description", "unknown error");
    }
    std::string result = handleSend.Write(CommunicationType::CHANGEPASSWORD);
    return result;
}

void Service::HandleFileTransfer(HandleRecv& handleRecv)
{
}
