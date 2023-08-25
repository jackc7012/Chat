#include "Service.h"

using namespace cwy;

Service::Service()
{
}

Service::~Service()
{
    exitFlag = true;
    for (unsigned short i = 0; i < threadHandle_.size(); ++i) {
        if (threadHandle_[i].joinable()) {
            threadHandle_[i].join();
        }
    }
    if (threadHeartBeat_.joinable()) {
        threadHeartBeat_.join();
    }
    if (dataBase_ != nullptr) {
        delete dataBase_;
        dataBase_ = nullptr;
    }
}

int Service::Init(CallBack callBack)
{
    int ret = -1;
    do {
        if (callBack == nullptr) {
            break;
        }
        SetEvent(callBack);
        GetInfo();
        InitLog();
        if (InitDataBase() != 0) {
            break;
        }
        SqlRequest sql("select max(ID) from tb_info");
        DataRecords loginInfo;
        if (!dataBase_->execSql(sql, &loginInfo)) {
            break;
        }
        if (loginInfo.size() != 0) {
            std::stringstream ID;
            ID << loginInfo.at(0).at(0);
            ID >> maxRegistered_;
            if (maxRegistered_ < DEFAULT_REGISTER_ID) {
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
    do {
        char name[256] = { 0 };
        int getNameRet = gethostname(name, sizeof(name));
        if (getNameRet == 0) {
            hostent* host = gethostbyname(name);
            if (NULL != host) {
                ip = std::string(inet_ntoa(*(in_addr*)*host->h_addr_list));
            }
        }
    } while (0);

    return ip;
}

SOCKET Service::StartTcp()
{
    do {
        socketServiceTcp_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socketServiceTcp_ == INVALID_SOCKET) {
            callBack_("init", "socket创建失败", true);
            break;
        }
        else {
            SOCKADDR_IN addrServiceTcp{ 0 };
            addrServiceTcp.sin_family = AF_INET;
            addrServiceTcp.sin_port = htons(info_.GetNetWorkInfo().tcpPort_);
            addrServiceTcp.sin_addr.S_un.S_addr = INADDR_ANY;
            if (::bind(socketServiceTcp_, (sockaddr*)&addrServiceTcp, sizeof(addrServiceTcp)) == SOCKET_ERROR) {
                ::closesocket(socketServiceTcp_);
                socketServiceTcp_ = INVALID_SOCKET;
                callBack_("init", "socket创建失败", true);
                break;
            }
            if (::listen(socketServiceTcp_, SOMAXCONN) == SOCKET_ERROR) {
                ::closesocket(socketServiceTcp_);
                socketServiceTcp_ = INVALID_SOCKET;
                callBack_("init", "socket创建失败", true);
                break;
            }
            int nNetTimeout = 100;
            ::setsockopt(socketServiceTcp_, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
            InitThread();
            //threadHeartBeat_ = std::thread(&Service::HandleHeartBeat, this);
        }
    } while (0);

    return socketServiceTcp_;
}

void Service::StopTcp()
{
    ::closesocket(socketServiceTcp_);
}

SOCKET Service::ClientAccept()
{
    SOCKET socketClient;
    do {
        SOCKADDR_IN sockAddr{ 0 };
        int addr_len = sizeof(sockAddr);
        socketClient = ::accept(socketServiceTcp_, (SOCKADDR*)&sockAddr, &addr_len);
        if (socketClient == INVALID_SOCKET) {
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
    do {
        if (socketAccept_.find(socketClient) == socketAccept_.end()) {
            break;
        }
        char* strRecv = new char[DATA_LENGTH];
        memset(strRecv, 0, DATA_LENGTH);
        int ret = ::recv(socketClient, strRecv, DATA_LENGTH, 0);
        if (ret > 0) {
            taskQue_.push(std::make_pair(strRecv, socketClient));
            delete[]strRecv;
            strRecv = nullptr;
        }
    } while (0);

    return ret;
}

void Service::SetEvent(CallBack callBack)
{
    if (callBack_ == nullptr) {
        callBack_ = callBack;
    }
}

void Service::GetInfo()
{
    info_.GetInfoFromFile(INFO_FILE_NAME);
}

void Service::InitLog()
{
    Log logInfo = info_.GetLogInfo();
    log_.InitLog(0, logInfo.path_, static_cast<LogLevel>(logInfo.level_), logInfo.size_, logInfo.autoFlush_);
}

void Service::InitThread()
{
    for (unsigned short i = 0; i < info_.GetCommonInfo().threadNum_; ++i) {
        threadHandle_.emplace_back(std::thread(&Service::ThreadHandler, this, i));
    }
}

int Service::InitDataBase()
{
    DataBase dataBaseInfo = info_.GetDataBaseInfo();
    dataBase_ = new DataBaseHandle();
    if (!dataBase_->initSqlDataBase(dataBaseInfo.ip_, dataBaseInfo.name_, dataBaseInfo.uid_, dataBaseInfo.pwd_)) {
        callBack_("init", "数据库连接失败！", true);
        return -1;
    }

    return 0;
}

void Service::ThreadHandler(const unsigned short threadNum)
{
    while (1) {
        if (exitFlag) {
            break;
        }
        std::string taskParam;
        std::pair<std::string, SOCKET> param;
        SOCKET socket;
        {
            std::lock_guard<std::mutex> lg(mutexPush_);
            if (taskQue_.empty()) {
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
    while (1) {

        Sleep(1);
    }
}

void Service::NetWorkEvent(const unsigned short threadNum, const std::string& taskParam, const SOCKET socket)
{
    s_HandleRecv handleRecv, handleSend;
    handleSend.socketAccept_ = socket;
    DecodeJson(taskParam, handleRecv);
    std::string result;
    std::unique_lock<std::mutex> ul(mutexHandle_, std::defer_lock);
    std::string ip;
    auto itor = socketAccept_.find(socket);
    if (itor != socketAccept_.end()) {
        ip = itor->second;
    }
    //callBack_("info", std::string(threadNum + " : recv from " + ip + " message : " + taskParam), false);
    switch (handleRecv.type_) {
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
        if (isLoginSucceed) {
            /*result = HandleShowLogin();
            ::send(socket, result.c_str(), result.length(), 0);
            result = HandleShowLogin(handleRecv.Param.login_.id, 1);
            auto itor = loginCustomer_.begin();
            for (; itor != loginCustomer_.end(); ++itor) {
                if (itor->second.second == socket) {
                    continue;
                }
                ::send(itor->second.second, result.c_str(), result.length(), 0);
            }
            ul.lock();
            ++loginCount_;
            loginCustomer_.insert(std::make_pair(handleRecv.Param.login_.id, std::make_pair(customerName, socket)));
            callBack_("login", std::string(std::to_string(handleRecv.Param.login_.id) + "|" + customerName + "|" + ip + "|在线"), false);
            ul.unlock();*/
        }
        break;
    }

    case CommunicationType::DELETECUSTOMER:
    {
        HandleExit(handleRecv.Param.delCustomer_.id);
        result = HandleShowLogin(handleRecv.Param.delCustomer_.id, -1);
        auto itor = loginCustomer_.begin();
        for (; itor != loginCustomer_.end(); ++itor) {
            if (itor->second.second == socket) {
                continue;
            }
            ::send(itor->second.second, result.c_str(), result.length(), 0);
        }
        ul.lock();
        --loginCount_;
        auto itor1 = loginCustomer_.find(handleRecv.Param.delCustomer_.id);
        if (itor1 != loginCustomer_.end()) {
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
        if (itor1 != loginCustomer_.end()) {
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
        if (itor == loginCustomer_.end()) {
            isOnline = false;
        }
        result = HandleChat(handleRecv, isOnline, handleSend);
        if (itor != loginCustomer_.end()) {
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

std::string Service::HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend)
{
    RegisterSpace(&handleSend.Param.registerBack_.customer, handleRecv.Param.register_.customer);
    if (strlen(handleRecv.Param.register_.password) <= 20) {
        //callBack_("register", handleSend.Param.registerBack_.customer +
            //std::string(" register failed, failed description : password to long"), true);
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
        RegisterSpace(&handleSend.Param.registerBack_.description, "password too short");
    }
    else {
        SqlRequest sql("insert into tb_info(ID, Name, Password, IsLogin, LastLoginTime, RegisterIp) values");
        sql << dbJoin(std::vector<std::string>{std::to_string(maxRegistered_), handleRecv.Param.register_.customer,
            handleRecv.Param.register_.password, "0", GetSystemTime(), ip});
        if (dataBase_->execSql(sql) == TRUE) {
            RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");
            handleSend.Param.registerBack_.id = maxRegistered_;
            sql.clear();
            sql << "create table [" << maxRegistered_ << "_Src"
                << "] ( TargetId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL)";
            if (dataBase_->execSql(sql) == FALSE) {
                //callBack_("register", handleSend.Param.registerBack_.customer +
                    //std::string(" create source table failed, failed description : ") + dataBase_->getErrMessage(), true);
            }
            sql.clear();
            sql << "create table [" << maxRegistered_ << "_Trg"
                << "] ( SourceId bigint NOT NULL, Time datetime NOT NULL, ChatContent text NOT NULL, IsRead bit NOT NULL)";
            if (dataBase_->execSql(sql) == FALSE) {
                //callBack_("register", handleSend.Param.registerBack_.customer +
                    //std::string(" create target table failed, failed description : ") + dataBase_->getErrMessage(), true);
            }
            ++maxRegistered_;
        }
        else {
            //callBack_("register", handleSend.Param.registerBack_.customer + std::string(" register failed, failed description : ")
                //+ dataBase_->getErrMessage(), true);
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

std::string Service::HandleLogin(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend,
    bool& isLoginSucceed, std::string& customerName)
{
    RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
    customerName = "";
    SqlRequest sql("select Name, Password, IsLogin, RecentIp from tb_info where ID = ");
    sql << handleRecv.Param.login_.id;
    DataRecords loginInfo;
    bool ret = dataBase_->execSql(sql, &loginInfo);
    if (!ret) {
        callBack_("login", std::to_string(handleRecv.Param.login_.id) + " select login fail, description : " +
            dataBase_->getErrMessage(), true);
    }
    else if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.loginBack_.description, "there is no such user");
    }
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(1)) != handleRecv.Param.login_.password) {
        RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
    }
    else if (loginInfo.size() == 1 && trim(loginInfo.at(0).at(2)) == "1") {
        delete[]handleRecv.Param.loginBack_.login_result;
        RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo.at(0).at(0));
        RegisterSpace(&handleSend.Param.loginBack_.login_result, ip);
        RegisterSpace(&handleSend.Param.loginBack_.description,
            "this user has already login in, please make sure or modify your password");
    }
    else if (loginInfo.size() == 1) {
        RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo.at(0).at(0));
        delete handleSend.Param.loginBack_.login_result;
        RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
        sql.clear();
        sql << "update tb_info set IsLogin = 1, RecentIp = " << toDbString(ip) << " where ID = " << handleRecv.Param.login_.id;
        if (dataBase_->execSql(sql) == FALSE) {
            callBack_("login", std::to_string(handleRecv.Param.login_.id) + " update login fail, description : " +
                dataBase_->getErrMessage(), true);
        }
        customerName = handleSend.Param.loginBack_.customer;
        isLoginSucceed = true;
    }
    else {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.loginBack_.login_result, "succeed") == 0)
        ? CommunicationType::LOGINBACKSUCCEED : CommunicationType::LOGINBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

void Service::HandleExit(const UINT64 id)
{
    SqlRequest sql;
    sql << "update tb_info set IsLogin = 0, LastLoginTime = " << toDbString(GetSystemTime())
        << " where ID = " << toDbString(std::to_string(id));
    if (dataBase_->execSql(sql) == FALSE) {
        callBack_("exit", "login out " + std::to_string(id) + " failed description : " + dataBase_->getErrMessage(), true);
    }
}

std::string Service::HandleChat(const s_HandleRecv& handleRecv, const bool isOnline, s_HandleRecv& handleSend)
{
    handleSend.type_ = CommunicationType::CHAT;
    // source table
    std::string sourceTableName(std::to_string(handleRecv.Param.chat_.sourceid) + "_Src");
    SqlRequest sql("insert into [");
    sql << sourceTableName << "] (TargetId, Time, ChatContent) values" << dbJoin({ std::to_string(handleRecv.Param.chat_.targetid),
        GetSystemTime(), handleRecv.Param.chat_.content });
    if (dataBase_->execSql(sql) == FALSE) {
        callBack_("chat", std::to_string(handleSend.Param.chat_.sourceid) + " insert source table [" + sourceTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }
    // target table
    std::string targetTableName(std::to_string(handleRecv.Param.chat_.targetid) + "_Trg");
    sql.clear();
    sql << "insert into [" << targetTableName << "] (SourceId, Time, ChatContent, IsRead) values"
        << dbJoin({ std::to_string(handleRecv.Param.chat_.sourceid), GetSystemTime(), handleRecv.Param.chat_.content,
            (isOnline ? "1" : "0") });
    if (dataBase_->execSql(sql) == FALSE) {
        callBack_("chat", std::to_string(handleSend.Param.chat_.sourceid) + " insert target table [" + targetTableName +
            "] failed, failed description : " + dataBase_->getErrMessage(), true);
    }

    if (isOnline) {
        RegisterSpace(&handleSend.Param.chat_.content, handleRecv.Param.chat_.content);
        handleSend.Param.chat_.sourceid = handleRecv.Param.chat_.sourceid;
        handleSend.Param.chat_.targetid = handleRecv.Param.chat_.targetid;
        std::string result = EncodeJson(handleSend.type_, handleSend);
        UnregisterSpace(handleSend.type_, handleSend);
        return result;
    }
    return "";
}

std::string Service::HandleShowLogin(const UINT64 id/* = -1*/, const int type/* = 0*/)
{
    std::string loginInfos;
    s_HandleRecv handleSend;
    handleSend.type_ = CommunicationType::SHOWLOGIN;
    handleSend.Param.showLogin_.show_login_type = type;
    if (type == -1 || type == 1) {
        auto itor = loginCustomer_.find(id);
        if (itor == loginCustomer_.end()) {
            return "";
        }
        loginInfos = CombineString({ {std::to_string(id), itor->second.first, ((type == -1) ? "0" : "1")} });
    }
    else {
        SqlRequest sql("select ID, Name, IsLogin from tb_info");
        DataRecords loginInfo;
        bool ret = dataBase_->execSql(sql, &loginInfo);
        if (!ret) {
            callBack_("show", std::to_string(id) + " select show login fail, description : " + dataBase_->getErrMessage(), true);
        }
        loginInfos = CombineString(loginInfo);
    }
    RegisterSpace(&handleSend.Param.showLogin_.customer, loginInfos);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}
