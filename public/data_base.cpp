#include "data_base.h"

using namespace cwy;

DataBaseHandle::DataBaseHandle()
{
}

DataBaseHandle::~DataBaseHandle()
{
    DataBaseImpl::createInstance()->uninitDataBase();
}

BOOL DataBaseHandle::initSqlDataBase(const std::string& ip, const std::string& dataBaseName, const std::string& uid, const std::string& pwd)
{
    return DataBaseImpl::createInstance()->initDataBase(ip, dataBaseName, uid, pwd);
}

std::string DataBaseHandle::getDbName() const
{
    return DataBaseImpl::createInstance()->getDbName();
}

std::string DataBaseHandle::getServerIp() const
{
    return DataBaseImpl::createInstance()->getServerIp();
}

std::string DataBaseHandle::getDbUid() const
{
    return DataBaseImpl::createInstance()->getDbUid();
}

BOOL DataBaseHandle::execSql(const SqlRequest& sqlRequest, DataRecords* result/* = nullptr*/)
{
    DBTYPE dbType = judgeCommand(sqlRequest.str());
    if ((result != nullptr) && (dbType == DBTYPE::SEARCH))
    {
        DataBaseImpl::createInstance()->selectSql(sqlRequest.str(), *result);
    }
    else
    {
        return DataBaseImpl::createInstance()->operSql(dbType, sqlRequest.str());
    }
    return TRUE;
}

BOOL DataBaseHandle::uninitDataBase()
{
    return DataBaseImpl::createInstance()->uninitDataBase();
}

std::string DataBaseHandle::getErrMessage() const
{
    return DataBaseImpl::createInstance()->getErrMessage();
}

DBTYPE DataBaseHandle::judgeCommand(const std::string& command)
{
    DBTYPE dbType;
    if (command.substr(0, command.find_first_of(' ')) == "create")
    {
        dbType = DBTYPE::CREATETABLE;
    }
    else if (command.substr(0, command.find_first_of(' ')) == "drop")
    {
        dbType = DBTYPE::DROPTABLE;
    }
    else if (command.substr(0, command.find_first_of(' ')) == "insert")
    {
        dbType = DBTYPE::INSERT;
    }
    else if (command.substr(0, command.find_first_of(' ')) == "delete")
    {
        dbType = DBTYPE::DEL;
    }
    else if (command.substr(0, command.find_first_of(' ')) == "update")
    {
        dbType = DBTYPE::MODIFY;
    }
    else if (command.substr(0, command.find_first_of(' ')) == "select")
    {
        dbType = DBTYPE::SEARCH;
    }
    return dbType;
}

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
    if (checkSqlValid(sqlRequest))
    {
        str_ << sqlRequest;
    }
    else
    {
        errType_ = ERRTYPE::COMMANDINJECTION;
    }
    return *this;
}

SqlRequest& SqlRequest::operator<<(const long long sqlRequest)
{
    std::string tmp = std::to_string(sqlRequest);
    if (checkSqlValid(tmp))
    {
        str_ << tmp;
    }
    else
    {
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

size_t SqlRequest::getLength() const
{
    return str_.str().length();
}
