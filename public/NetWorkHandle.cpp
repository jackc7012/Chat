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

std::string NetWorkHandle::HandleLogin(const s_HandleRecv& handleRecv)
{
    s_HandleRecv result;
    std::string strResult;
    SqlRequest sql("select Password, IsLogin from tb_info where Name = ");
    sql << toDbString(handleRecv.Param.login_.customer);
    std::vector<std::vector<std::string>> loginInfo = dataBase->selectDbInfo(sql);
    if (loginInfo.size() == 0) {
        result.Param.loginBack_.description = _T("查无此用户");
    } else if (loginInfo[0][0] != handleRecv.Param.login_.password) {
        return "";
    } else {
        sql.clear();
        sql << "update tb_info set IsLogin = 1 where Name = " << toDbString(handleRecv.Param.login_.customer);
        dataBase->updateDbInfo(sql);
    }
    return result;
}
