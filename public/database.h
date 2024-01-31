#ifndef __DATA_BASE_H__
#define __DATA_BASE_H__

#include <string>
#include <vector>

#include "public.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

namespace cwy
{
    enum class ERRTYPE
    {
        OK,
        COMMANDINJECTION
    };

    class SqlRequest
    {
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
        size_t getLength() const;
        ERRTYPE getErr() const;

    private:
        std::stringstream str_;
        ERRTYPE errType_{ ERRTYPE::OK };
    };

    enum class DBTYPE
    {
        CREATETABLE,
        DROPTABLE,
        INSERT,
        DEL,
        MODIFY,
        SEARCH
    };

    class DataBase
    {
    public:
        static DataBase* createInstance();

        ~DataBase();

        BOOL initDataBase(const std::string& ip, const std::string& dataBaseName, const std::string& uid, const std::string& pwd);

        BOOL operSql(const std::string& sqlRequest);

        BOOL selectSql(const std::string& sqlRequest, DataRecords& result);

        BOOL uninitDataBase();

        std::string getDbName() const
        {
            return dataBaseName_;
        }

        std::string getServerIp() const
        {
            return dataBaseIp_;
        }

        std::string getDbUid() const
        {
            return dataBaseUid_;
        }

        std::string getErrMessage() const
        {
            return errMessage_;
        }

    private:
        DataBase();

        DataBase(const DataBase&) = delete;
        DataBase(DataBase&&) = delete;
        DataBase operator=(const DataBase&) = delete;
        DataBase operator=(DataBase&&) = delete;

    private:
        _ConnectionPtr pMyConnect{ nullptr };
        _RecordsetPtr pRecordset{ nullptr };
        std::string dataBaseName_;
        std::string dataBaseIp_;
        std::string dataBaseUid_;
        std::string errMessage_;
    };
}

#endif // !__DATA_BASE_H__