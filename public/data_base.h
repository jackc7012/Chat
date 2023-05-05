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

        /* ���ݿ��ʼ��
            * ip:���ݿ������ip
            * dataBaseName:���ݿ���
            * uid:���ݿ��¼�û���
            * pwd:���ݿ��¼����
            * return ���ӳɹ����
            */
        BOOL initSqlDataBase(const std::string& ip, const std::string& dataBaseName, const std::string& uid, const std::string& pwd);

        /* ��ȡ���ݿ���
            * return ���ݿ���
            */
        std::string getDbName() const;

        /* ��ȡ���ݿ������ip
            * return ������ip
            */
        std::string getServerIp() const;

        /* ��ȡ���ݿ��¼�û���
            * return ��������¼�û���
            */
        std::string getDbUid() const;

        BOOL execSql(const SqlRequest& sqlRequest, DataRecords* result = nullptr);

        /* ���ݿⷴ��ʼ��
            * return ����ʼ���ɹ����
            */
        BOOL uninitDataBase();

        /* ��ȡ���ݿ�ʧ�ܲ�����Ϣ
            * return ������Ϣ
            */
        std::string getErrMessage() const;

    private:
        DBTYPE judgeCommand(const std::string& command);
    };
}

#endif // !MY_DATA_BASE_H
