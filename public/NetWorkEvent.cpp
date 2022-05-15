#include "NetWorkEvent.h"

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
        std::string errMsg;
        dataBase = std::make_unique<DataBaseImpl>();
        dataBase->initDataBase(ip, name, errMsg);
    }
    return 0;
}

void NetWorkEvent::NetWorkEventHandle(const s_HandleRecv& taskContent, CommunicationType& type, std::string& msg)
{
    s_HandleRecv handleSend;
    switch (taskContent.type_) {
        case CommunicationType::REGISTER:
        {
            NetWorkRegisterHandle(taskContent, handleSend);
            break;
        }

        case CommunicationType::LOGIN:
        {
            NetWorkLoginHandle(taskContent, handleSend);
            break;
        }

        case CommunicationType::DELETECUSTOMER:
        {
            NetWorkDelHandle(taskContent);
            msg = taskContent.Param.delCustomer_.customer;
            type = CommunicationType::DELETECUSTOMER;
            break;
        }
    }
    msg = ((msg.empty()) ? (EncodeJson(handleSend.type_, handleSend)) : (msg));
    type = ((type == CommunicationType::NULLCOMMUNICATION) ? (handleSend.type_) : (type));
    UnregisterSpace(handleSend.type_, handleSend);
}

void NetWorkEvent::NetWorkRegisterHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend)
{
    std::string errMsg;
    RegisterSpace(&handleSend.Param.registerBack_.customer, taskContent.Param.register_.customer);
    RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
    SqlRequest sql("select 1 from tb_info where Name = ");
    sql << ToDbString(taskContent.Param.register_.customer);
    DataBaseRecord loginInfo;
    dataBase->selectSql(sql.str(), loginInfo, errMsg);
    if (loginInfo.size() == 1) {
        RegisterSpace(&handleSend.Param.registerBack_.description, "user name has already exist");
    } else if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");
        sql.clear();
        sql << "insert into tb_info(Name, Password, Ip, IsLogin) values"
            << DbJoin(std::vector<std::string>{taskContent.Param.register_.customer, taskContent.Param.register_.password,
                                               taskContent.connect_ip_, "0"});
        dataBase->operSql(DBTYPE::INSERT, sql.str(), errMsg);
        sql.clear();
        sql << "create table [" << taskContent.connect_ip_
            << "] ( Time datetime NOT NULL,"
            << "   Target varchar(50) NOT NULL,"
            << "   ChatContent text NOT NULL)";
        dataBase->operSql(DBTYPE::CREATETABLE, sql.str(), errMsg);
    } else {
        RegisterSpace(&handleSend.Param.registerBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
        ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
}

void NetWorkEvent::NetWorkLoginHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend)
{
    std::string errMsg;
    RegisterSpace(&handleSend.Param.loginBack_.customer, taskContent.Param.login_.customer);
    RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
    SqlRequest sql("select Password from tb_info where Name = ");
    sql << ToDbString(taskContent.Param.login_.customer);
    DataBaseRecord loginInfo;
    dataBase->selectSql(sql.str(), loginInfo, errMsg);
    if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.loginBack_.description, "no such user name");
    } else if (loginInfo.size() == 1) {
        if (loginInfo[0][0] != taskContent.Param.login_.password) {
            RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
        } else {
            RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
            sql.clear();
            sql << "update tb_info set IsLogin = '1' where Name = " << ToDbString(taskContent.Param.login_.customer);
            dataBase->operSql(DBTYPE::MODIFY, sql.str(), errMsg);
        }
    } else {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.loginBack_.login_result, "succeed") == 0)
        ? CommunicationType::LOGINBACKSUCCEED : CommunicationType::LOGINBACKFAILED);
}

void NetWorkEvent::NetWorkDelHandle(const s_HandleRecv& taskContent)
{
    std::string errMsg;
    SqlRequest sql("update tb_info set IsLogin = '0' where Name = ");
    sql << ToDbString(taskContent.Param.delCustomer_.customer);
    dataBase->operSql(DBTYPE::MODIFY, sql.str(), errMsg);
}
}
