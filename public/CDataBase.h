#ifndef __DATA_BASE_H__
#define __DATA_BASE_H__

#include <windows.h>
#include <string>

#include "CLog.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

namespace cwy {
    class CDataBase
    {
    public:
        static CDataBase* CreateInstance();

        ~CDataBase();

        int InitDataBase(const std::string& dataBaseName);

        void GetId();

        int SearchDataBaseLogin(const long long loginID, std::string& name, std::string& ip, char* password, int& loginStatus);

        int UpdateLoginStatus(const int type = 0, const long long loginID = -1);

        long long InsertRegister(const std::string& registerName, const char* password, const std::string ip);

    private:
        CDataBase();

        CDataBase(const CDataBase&) = delete;
        CDataBase operator=(const CDataBase&) = delete;

    private:
        _ConnectionPtr pMyConnect{ nullptr };
        _RecordsetPtr pRecordset{ nullptr };
        CLog logDataBase;
        long long presentId{ 60000 };
    };
}

#endif // !__DATA_BASE_H__