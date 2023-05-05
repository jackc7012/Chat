#ifndef MY_DATA_BASE_H
#define MY_DATA_BASE_H

#include <Windows.h>
#include <sstream>
#include <vector>

#include "data_interface.h"

namespace cwy {
    enum class ERRTYPE {
        OK,
        COMMANDINJECTION
    };

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
        size_t getLength() const;

    private:
        std::stringstream str_;
        ERRTYPE errType_{ ERRTYPE::OK };
    };

    class DataBaseHandle {
    public:
        DataBaseHandle();
        ~DataBaseHandle();

        DataBaseHandle(const DataBaseHandle&) = delete;
        DataBaseHandle(DataBaseHandle&&) = delete;
        DataBaseHandle& operator = (const DataBaseHandle&) = delete;
        DataBaseHandle& operator = (DataBaseHandle&&) = delete;

        /* 数据库初始化
            * ip:数据库服务器ip
            * dataBaseName:数据库名
            * uid:数据库登录用户名
            * pwd:数据库登录密码
            * return 连接成功与否
            */
        BOOL initSqlDataBase(const std::string& ip, const std::string& dataBaseName, const std::string& uid, const std::string& pwd);

        /* 获取数据库名
            * return 数据库名
            */
        std::string getDbName() const;

        /* 获取数据库服务器ip
            * return 服务器ip
            */
        std::string getServerIp() const;

        /* 获取数据库登录用户名
            * return 服务器登录用户名
            */
        std::string getDbUid() const;

        BOOL execSql(const SqlRequest& sqlRequest, DataRecords* result = nullptr);

        /* 数据库反初始化
            * return 反初始化成功与否
            */
        BOOL uninitDataBase();

        /* 获取数据库失败操作信息
            * return 错误信息
            */
        std::string getErrMessage() const;

    private:
        DBTYPE judgeCommand(const std::string& command);
    };
}

#endif // !MY_DATA_BASE_H
