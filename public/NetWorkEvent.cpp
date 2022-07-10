#include "NetWorkEvent.h"

#include "CLog.h"

namespace cwy {
NetWorkEvent::~NetWorkEvent()
{
    if (dataBase != nullptr) {
        dataBase->uninitDataBase();
        dataBase.reset();
        dataBase = nullptr;
    }
}

int NetWorkEvent::InitDataBase(const std::string& ip, const std::string& name)
{
    if (dataBase == nullptr) {
        dataBase = std::make_unique<DataBaseImpl>();
        dataBase->initDataBase(ip, name);
    }
    logServiceNetwork.InitLog("../{time}/network");
    return 0;
}

SOCKET NetWorkEvent::NetWorkEventHandle(const s_HandleRecv& taskContent, CommunicationType& type, std::string& msg)
{
    s_HandleRecv handleSend;
    SOCKET socket{INVALID_SOCKET};
    switch (taskContent.type_) {
        case CommunicationType::REGISTER:
        {
            socket = NetWorkRegisterHandle(taskContent, handleSend);
            break;
        }

        case CommunicationType::LOGIN:
        {
            socket = NetWorkLoginHandle(taskContent, handleSend);
            break;
        }

        case CommunicationType::DELETECUSTOMER:
        {
            socket = NetWorkDelHandle(taskContent);
            msg = taskContent.Param.delCustomer_.id;
            type = CommunicationType::DELETECUSTOMER;
            break;
        }

        case CommunicationType::CHAT:
        {
            socket = NetWorkChatHandle(taskContent, handleSend);
            break;
        }
    }
    msg = ((msg.empty()) ? (EncodeJson(handleSend.type_, handleSend)) : (msg));
    type = ((type == CommunicationType::NULLCOMMUNICATION) ? (handleSend.type_) : (type));
    logServiceNetwork << msg;
    logServiceNetwork.PrintlogInfo(FILE_FORMAT);
    UnregisterSpace(handleSend.type_, handleSend);
    return socket;
}

SOCKET NetWorkEvent::NetWorkRegisterHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend)
{
    try {
        SqlRequest sql("select max(Id) from tb_info");
        DataBaseRecord loginInfo;
        dataBase->selectSql(sql.str(), loginInfo);
        unsigned long long id = ((loginInfo.size() == 0) ? 9999 : atoll(loginInfo[0][0].c_str()));
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");

        sql.clear();
        sql << "insert into tb_info(Id, Name, Password, Ip, IsLogin) values"
            << DbJoin(StringMap{{1, taskContent.Param.register_.customer}, {2, taskContent.Param.register_.password},
                                {3, taskContent.connect_ip_}}, IntMap{{0, ++id}, {4, 0}});
        dataBase->operSql(DBTYPE::INSERT, sql.str());

        sql.clear();
        sql << "create table [" << id << "_" << taskContent.connect_ip_
            << "] ( Time datetime NOT NULL, Source varchar(50) NOT NULL, Target varchar(50) NOT NULL,"
            << "    TargetIp varchar(50) NOT NULL, ChatContent text NOT NULL)";
        dataBase->operSql(DBTYPE::CREATETABLE, sql.str());

        handleSend.Param.registerBack_.id = id;
        handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
            ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
    }
    catch (...) {
        return INVALID_SOCKET;
    }
    return taskContent.socket_accept_;
}

SOCKET NetWorkEvent::NetWorkLoginHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend)
{
    try {
        long long id = taskContent.Param.login_.id;
        RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
        SqlRequest sql("select Id, Name, Password from tb_info where Id = ");
        sql << taskContent.Param.login_.id;
        DataBaseRecord loginInfo;
        dataBase->selectSql(sql.str(), loginInfo);
        if (loginInfo.size() == 0) {
            RegisterSpace(&handleSend.Param.loginBack_.description, "no such id");
        } else if (loginInfo.size() == 1) {
            if (loginInfo[0][2] != taskContent.Param.login_.password) {
                RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
            } else {
                RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
                RegisterSpace(&handleSend.Param.loginBack_.customer, loginInfo[0][1]);
                sql.clear();
                sql << "update tb_info set IsLogin = 1 where Id = " << taskContent.Param.login_.id;
                dataBase->operSql(DBTYPE::MODIFY, sql.str());
                IpAndSocket ipAndSocket{taskContent.connect_ip_, taskContent.socket_accept_};
                idToIpSock.insert(std::make_pair(loginInfo[0][0], ipAndSocket));
            }
        } else {
            RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
        }
        handleSend.type_ = ((strcmp(handleSend.Param.loginBack_.login_result, "succeed") == 0)
            ? CommunicationType::LOGINBACKSUCCEED : CommunicationType::LOGINBACKFAILED);
    }
    catch (...) {
        return INVALID_SOCKET;
    }
    return taskContent.socket_accept_;
}

SOCKET NetWorkEvent::NetWorkDelHandle(const s_HandleRecv& taskContent)
{
    try {
        std::string delTime = GetTime(0);
        SqlRequest sql("update tb_info set IsLogin = 0, LastLoginTime = convert(datetime, ");
        sql << ToDbString(delTime) << ") where Id = " << ToDbString(taskContent.Param.delCustomer_.id);
        dataBase->operSql(DBTYPE::MODIFY, sql.str());
        auto itor = idToIpSock.find(std::to_string(taskContent.Param.delCustomer_.id));
        if (itor != idToIpSock.end()) {
            idToIpSock.erase(itor);
        }
    }
    catch (...) {
        return INVALID_SOCKET;
    }
    return taskContent.socket_accept_;
}

SOCKET NetWorkEvent::NetWorkChatHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend)
{
    SOCKET socket = INVALID_SOCKET;
    try {
        auto itor = idToIpSock.find(taskContent.Param.chat_.target);
        if (itor == idToIpSock.end()) {
            handleSend.type_ = CommunicationType::ERRORCOMMUNICATION;
            return INVALID_SOCKET;
        }
        RegisterSpace(&handleSend.Param.chat_.source, taskContent.Param.chat_.source);
        RegisterSpace(&handleSend.Param.chat_.target, (std::string(taskContent.Param.chat_.target) + "-" + itor->second.first));
        RegisterSpace(&handleSend.Param.chat_.content, taskContent.Param.chat_.content);
        std::string chatTime = GetTime(0);
        SqlRequest sql("insert into [");
        sql << taskContent.Param.chat_.source << "_" << taskContent.connect_ip_ << "](Time, Source, Target, TargetIp, ChatContent)"
            << " values(convert(datetime, " << ToDbString(chatTime) << "), " << ToDbString(taskContent.Param.chat_.source)
            << ", " << ToDbString(taskContent.Param.chat_.target) << ", " << ToDbString(itor->second.first) << ", "
            << ToDbString(taskContent.Param.chat_.content) << ")";
        dataBase->operSql(DBTYPE::INSERT, sql.str());
        handleSend.type_ = CommunicationType::CHAT;
        socket = itor->second.second;
    }
    catch (...) {

    }
    return socket;
}
}
