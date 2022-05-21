#ifndef __DATA_BASE_H__
#define __DATA_BASE_H__

#include "public.h"
#include <string>
#include <sstream>
#include <vector>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

enum class DBTYPE {
    CREATETABLE,
    DROPTABLE,
    INSERT,
    DEL,
    MODIFY,
    SEARCH
};

enum class ERRTYPE {
    OK,
    COMMANDINJECTION
};

namespace cwy {
class SqlRequest {
public:
    SqlRequest(const std::string& str);
    SqlRequest();
    ~SqlRequest();
    SqlRequest(const SqlRequest&) = delete;
    SqlRequest(SqlRequest&&) = delete;
    SqlRequest& operator = (const SqlRequest&) = delete;
    SqlRequest& operator = (SqlRequest&&) = delete;

    SqlRequest& operator <<(const std::string& sqlRequest);
    SqlRequest& operator <<(const long long sqlRequest);

    void clear();
    std::string str() const;

private:
    std::stringstream str_;
    ERRTYPE errType_{ERRTYPE::OK};
};

using DataBaseRecord = std::vector<std::vector<std::string>>;
class DataBaseImpl {
public:
    DataBaseImpl();

    ~DataBaseImpl();

    BOOL initDataBase(const std::string& ip, const std::string& dataBaseName);

    BOOL operSql(const DBTYPE dbType, const std::string& sqlRequest);

    void selectSql(const std::string& sqlRequest, DataBaseRecord& result);

    BOOL uninitDataBase();

    std::string getDbName() const
    {
        return dataBaseName_;
    }

    std::string getServerIp() const
    {
        return dataBaseIp_;
    }

private:
    BOOL judgeCommand(const DBTYPE dbType, const std::string& command);

private:
    DataBaseImpl(const DataBaseImpl&) = delete;
    DataBaseImpl(DataBaseImpl&&) = delete;
    DataBaseImpl operator=(const DataBaseImpl&) = delete;
    DataBaseImpl operator=(DataBaseImpl&&) = delete;

private:
    _ConnectionPtr pMyConnect{nullptr};
    _RecordsetPtr pRecordset{nullptr};
    std::string dataBaseName_;
    std::string dataBaseIp_;
};
}

#endif // !__DATA_BASE_H__