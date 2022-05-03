#include "NetWorkHandle.h"

using namespace cwy;

NetWorkHandle::NetWorkHandle()
:   dataBase(std::make_unique<DataBase>())
{
}

int NetWorkHandle::InitDataBase(const std::string& ip, const std::string& name)
{
    if (!dataBase->initSqlDataBase("127.0.0.1", "MyChat")) {
        return -1;
    }
    return 0;
}

std::string NetWorkHandle::HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend)
{
    RegisterSpace(&handleSend.Param.registerBack_.customer, handleRecv.Param.login_.customer);
    RegisterSpace(&handleSend.Param.registerBack_.register_result, "failed");
    SqlRequest sql("select 1 from tb_info where Name = ");
    sql << toDbString(handleRecv.Param.register_.customer);
    std::vector<std::vector<std::string>> loginInfo = dataBase->selectDbInfo(sql);
    if (loginInfo.size() == 1) {
        RegisterSpace(&handleSend.Param.registerBack_.description, "user name has already exist");
    } else if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.registerBack_.register_result, "succeed");
        SqlRequest sql("insert into tb_info(Name, Password, Ip, IsLogin) value");
        sql << dbJoin(std::vector<std::string>{handleRecv.Param.login_.customer, handleRecv.Param.login_.password, ip, "0"});
        dataBase->insertDbInfo(sql);
    }else {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.registerBack_.register_result, "succeed") == 0)
                         ? CommunicationType::REGISTERBACKSUCCEED : CommunicationType::REGISTERBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

std::string NetWorkHandle::HandleLogin(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend, bool& isLoginSucceed)
{
    RegisterSpace(&handleSend.Param.loginBack_.customer, handleRecv.Param.login_.customer);
    RegisterSpace(&handleSend.Param.loginBack_.login_result, "failed");
    SqlRequest sql("select Password, IsLogin from tb_info where Name = ");
    sql << toDbString(handleRecv.Param.login_.customer);
    std::vector<std::vector<std::string>> loginInfo = dataBase->selectDbInfo(sql);
    if (loginInfo.size() == 0) {
        RegisterSpace(&handleSend.Param.loginBack_.description, "there is no such user");
    } else if (loginInfo.size() == 1 && loginInfo[0][1] == "1") {
        RegisterSpace(&handleSend.Param.loginBack_.description,
                      "this user has already login in, please make sure or modify your password");
    } else if (loginInfo.size() == 1 && loginInfo[0][0] != handleRecv.Param.login_.password) {
        RegisterSpace(&handleSend.Param.loginBack_.description, "password error");
    } else if (loginInfo.size() == 1){
        RegisterSpace(&handleSend.Param.loginBack_.login_result, "succeed");
        sql.clear();
        sql << "update tb_info set IsLogin = 1 where Name = " << toDbString(handleRecv.Param.login_.customer);
        dataBase->updateDbInfo(sql);
        isLoginSucceed = true;
    } else {
        RegisterSpace(&handleSend.Param.loginBack_.description, "unkown error");
    }
    handleSend.type_ = ((strcmp(handleSend.Param.loginBack_.login_result, "succeed") == 0)
                         ? CommunicationType::LOGINBACKSUCCEED : CommunicationType::LOGINBACKFAILED);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}

void NetWorkHandle::HandleExit(const std::string& customer)
{
    SqlRequest sql;
    sql << "update tb_info set IsLogin = 0 where Name = " << toDbString(customer);
    dataBase->updateDbInfo(sql);
}

std::string NetWorkHandle::HandleChat(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend)
{
    handleSend.type_ = CommunicationType::CHAT;
    RegisterSpace(&handleSend.Param.chat_.content, handleRecv.Param.chat_.content);
    RegisterSpace(&handleSend.Param.chat_.source, handleRecv.Param.chat_.source);
    RegisterSpace(&handleSend.Param.chat_.target, handleRecv.Param.chat_.target);
    std::string result = EncodeJson(handleSend.type_, handleSend);
    UnregisterSpace(handleSend.type_, handleSend);
    return result;
}
