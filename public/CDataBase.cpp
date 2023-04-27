#include "CDataBase.h"

#include "public.h"
using namespace cwy;

SqlRequest::SqlRequest(const std::string& str)
{
    str_ << str;
}

SqlRequest::SqlRequest()
{
    str_.str("");
}

SqlRequest::~SqlRequest()
{
    str_.clear();
}

SqlRequest& SqlRequest::operator<<(const std::string& sqlRequest)
{
    if (CheckSqlValid(sqlRequest)) {
        str_ << sqlRequest;
    } else {
        errType_ = ERRTYPE::COMMANDINJECTION;
    }
    return *this;
}

SqlRequest& SqlRequest::operator<<(const long long sqlRequest)
{
    std::string tmp = std::to_string(sqlRequest);
    if (CheckSqlValid(tmp)) {
        str_ << tmp;
    } else {
        errType_ = ERRTYPE::COMMANDINJECTION;
    }
    return *this;
}

void SqlRequest::clear()
{
    str_.str("");
}

std::string SqlRequest::str() const
{
    return str_.str();
}

DataBaseImpl::DataBaseImpl()
{

}

DataBaseImpl::~DataBaseImpl()
{
}

BOOL DataBaseImpl::initDataBase(const std::string& ip, const std::string& dataBaseName)
{
    try {
        dataBaseName_ = dataBaseName;
        dataBaseIp_ = ip;
        HRESULT result = CoInitialize(NULL);
        if (result != S_OK) {
            return FALSE;
        }
        if (pMyConnect.CreateInstance(__uuidof(Connection)) != 0) {
            return FALSE;
        }
        if (pRecordset.CreateInstance(__uuidof(Recordset)) != 0) {
            return FALSE;
        }
        char connectionString[100] = {0};
        sprintf_s(connectionString, 100, "Driver={sql server};server=%s;uid=sa;pwd=sa;database=%s;",
            ip.c_str(), dataBaseName.c_str());
        if (pMyConnect->Open(connectionString, "", "", adModeUnknown) != 0) {
            return FALSE;
        }
        return TRUE;
    }
    catch (_com_error e) {
        throw e.Description();
        return FALSE;
    }
}

BOOL DataBaseImpl::operSql(const DBTYPE dbType, const std::string& sqlRequest)
{
    try {
        if (!judgeCommand(dbType, sqlRequest)) {
            return FALSE;
        }
        pRecordset = pMyConnect->Execute(sqlRequest.c_str(), NULL, adCmdText);
    }
    catch (_com_error e) {
        throw e.Description();
        return FALSE;
    }
    return TRUE;
}

void DataBaseImpl::selectSql(const std::string& sqlRequest, DataBaseRecord& result)
{
    try {
        pRecordset = pMyConnect->Execute(sqlRequest.c_str(), NULL, adCmdText);
        while (!pRecordset->adoEOF) {
            long fieldCount = pRecordset->GetFields()->Count;
            std::vector<std::string> fieldRecord;
            for (long j = 0; j < fieldCount; ++j) {
                _variant_t tmp = pRecordset->GetFields()->GetItem((long)j)->GetValue();
                if (tmp.vt == VT_NULL) {
                    continue;
                }
                fieldRecord.emplace_back((std::string)(_bstr_t)&tmp);
            }
            if (fieldRecord.size() != 0) {
                result.emplace_back(fieldRecord);
            }
            pRecordset->MoveNext();
        }
    }
    catch (_com_error e) {
        throw e.Description();
        return;
    }
}

BOOL DataBaseImpl::uninitDataBase()
{
    try {
        pRecordset.Release();
        pRecordset = nullptr;
        pMyConnect->Close();
        pMyConnect.Release();
        pMyConnect = nullptr;
        CoUninitialize();
    }
    catch (_com_error e) {
        return FALSE;
    }

    return TRUE;
}

BOOL DataBaseImpl::judgeCommand(const DBTYPE dbType, const std::string& command)
{
    static std::unordered_map<DBTYPE, std::string> typeMap{
        {DBTYPE::CREATETABLE, "create table"},
        {DBTYPE::DROPTABLE, "drop table"},
        {DBTYPE::INSERT, "insert into"},
        {DBTYPE::MODIFY, "update"},
        {DBTYPE::DEL, "delete from"}
    };
    if (command.substr(0, typeMap[dbType].length()) == typeMap[dbType]) {
        return TRUE;
    } else {
        return FALSE;
    }
}
