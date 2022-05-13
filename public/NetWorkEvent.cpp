#include "NetWorkEvent.h"

namespace cwy {
NetWorkEvent::~NetWorkEvent()
{
    if (dataBase != nullptr) {
        delete dataBase;
        dataBase = nullptr;
    }
}

int NetWorkEvent::InitDataBase(const std::string& ip, const std::string& name)
{
    if (dataBase == nullptr) {
        dataBase = new DataBase();
        dataBase->initSqlDataBase(ip, name);
    }
    return 0;
}

void NetWorkEvent::NetWorkEventHandle(const s_HandleRecv& taskContent, int& code, std::string& msg)
{
    switch (taskContent.type_) {
        case CommunicationType::REGISTER:
        {
            NetWorkRegisterHandle(taskContent, code, msg);
            break;
        }
    }
}

void NetWorkEvent::NetWorkRegisterHandle(const s_HandleRecv& taskContent, int code, std::string& msg)
{
    s_HandleRecv handleSend;
    RegisterSpace(&handleSend.Param.registerBack_.customer, taskContent.Param.login_.customer);
    RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
    SqlRequest sql("select 1 from tb_info where Name = ");
    sql << toDbString(taskContent.Param.register_.customer);
    std::vector<std::vector<std::string>> loginInfo = dataBase->selectDbInfo(sql);
    if (loginInfo.size() == 1) {
        RegisterSpace(&handleSend.Param.registerBack_.description, "user name has already exist");
    } else if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");
        SqlRequest sql("insert into tb_info(Name, Password, Ip, IsLogin) value");
        sql << dbJoin(std::vector<std::string>{taskContent.Param.login_.customer, taskContent.Param.login_.password,
                                               taskContent.connect_ip_, "0"});
        dataBase->insertDbInfo(sql);
    } else {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
        ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
    msg = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
}
}
