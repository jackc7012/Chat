#include "CDataBase.h"
using namespace cwy;

CDataBase::CDataBase()
{
    logDataBase.InitLog("../{time}/database");
}

CDataBase* CDataBase::CreateInstance()
{
    static CDataBase* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = new CDataBase();
    }
    return ptr;
}

CDataBase::~CDataBase()
{
    CoUninitialize();
}

int CDataBase::InitDataBase(const std::string& dataBaseName)
{
    HRESULT result =  CoInitialize(NULL);
    if (result != S_OK) {
        logDataBase << "init com error";
        logDataBase.PrintlogError(FILE_FORMAT);
        return -1;
    }
    if (pMyConnect.CreateInstance(__uuidof(Connection)) != 0) {
        logDataBase << "ConnectionPtr create failed";
        logDataBase.PrintlogError(FILE_FORMAT);
        return -1;
    }
    if (pRecordset.CreateInstance(__uuidof(Recordset)) != 0) {
        logDataBase << "RecordsetPtr create failed";
        logDataBase.PrintlogError(FILE_FORMAT);
        return -1;
    }
    char connectionString[100] = { 0 };
    sprintf_s(connectionString, 100, "Driver={sql server};server=127.0.0.1;uid=sa;pwd=chen931125;database=%s;",	                    dataBaseName.c_str());
    if (pMyConnect->Open(connectionString, "", "", adModeUnknown) != 0) {
        logDataBase << "open database" << dataBaseName.c_str() << "failed";
        logDataBase.PrintlogError(FILE_FORMAT);
        return -1;
    }
    GetId();
    return 0;
}

void CDataBase::GetId()
{
    char sql[100] = { 0 };
    sprintf_s(sql, 100, "select ID from tb_info order by ID desc");
    try
    {
        pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
        _variant_t id = pRecordset->GetFields()->GetItem((long)0)->GetValue();
        presentId = atoll(((std::string)(_bstr_t)&id).c_str());
    }
    catch (_com_error e)
    {
        logDataBase << "GetId failed descripton :" << (std::string)e.Description();
        logDataBase.PrintlogError(FILE_FORMAT);
    }
    return ;
}

int CDataBase::SearchDataBaseLogin(const long long loginID, std::string& name, std::string& ip, char* password, int& loginStatus)
{
    int result = 0;
    char sql[100] = { 0 };
    sprintf_s(sql, 100, "select Name, Password, Ip, IsLogin from tb_info where ID = %lld", loginID);
    try
    {
        logDataBase << "sql :" << sql;
        logDataBase.PrintlogDebug(FILE_FORMAT);
        pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
        _variant_t sqlName = pRecordset->GetFields()->GetItem((long)0)->GetValue();
        _variant_t sqlPassword = pRecordset->GetFields()->GetItem((long)1)->GetValue();
        _variant_t sqlIp = pRecordset->GetFields()->GetItem((long)2)->GetValue();
        _variant_t sqlIsLogin = pRecordset->GetFields()->GetItem((long)3)->GetValue();
        memcpy_s(password, 50, (char*)(_bstr_t)&sqlPassword, strlen((_bstr_t)&sqlPassword) + 1);
        name = (std::string)(_bstr_t)&sqlName;
        ip = (std::string)(_bstr_t)&sqlIp;
        loginStatus = atoi(((std::string)(_bstr_t)&sqlIsLogin).c_str());
    }
    catch (_com_error e)
    {
        logDataBase << "SearchDataBaseLogin failed descripton :" << (std::string)e.Description();
        logDataBase.PrintlogError(FILE_FORMAT);
        result = -1;
    }
    return result;
}

int CDataBase::UpdateLoginStatus(const int type/* = 0*/, const long long loginID/* = -1*/)
{
    int result = 0;
    char sql[100] = { 0 };
    if (loginID == -1) {
        sprintf_s(sql, 100, "update tb_info set IsLogin = %d ", type);
    }
    else {
        sprintf_s(sql, 100, "update tb_info set IsLogin = %d where ID = %lld", type, loginID);
    }
    try
    {
        logDataBase << "sql :" << sql;
        logDataBase.PrintlogDebug(FILE_FORMAT);
        pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
    }
    catch (_com_error e)
    {
        logDataBase << "UpdateLoginStatus " << type << " failed descripton :" << (std::string)e.Description();
        logDataBase.PrintlogError(FILE_FORMAT);
        result = -1;
    }
    return result;
}

long long CDataBase::InsertRegister(const std::string& registerName, const char* password, const std::string ip)
{
    long long result = 0;
    char sql[100] = { 0 };
    sprintf_s(sql, 100, "insert into tb_info values(%lld, '%s', '%s', '%s', 0)", ++presentId, registerName.c_str(), password, ip.c_str());
    try
    {
        logDataBase << "sql :" << sql;
        logDataBase.PrintlogDebug(FILE_FORMAT);
        pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
        result = presentId;
    }
    catch (_com_error e)
    {
        logDataBase << "InsertRegister failed descripton :" << (std::string)e.Description();
        logDataBase.PrintlogError(FILE_FORMAT);
        result = -1;
    }
    return result;
}
