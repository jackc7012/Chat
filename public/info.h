#ifndef __MY_INFO_H__
#define __MY_INFO_H__

#include <string>

namespace cwy {
    struct NetWork {
        NetWork()
            : tcpPort_(0)
            , udpPort_(0)
        {
            memset(ip_, 0, 50);
        }
        char ip_[50];
        unsigned int tcpPort_;
        unsigned int udpPort_;
    };

    struct DataBase {
        DataBase()
        {
            memset(ip_, 0, 50);
            memset(name_, 0, 50);
            memset(uid_, 0, 50);
            memset(pwd_, 0, 50);
        }
        char ip_[50];
        char name_[50];
        char uid_[50];
        char pwd_[50];
    };

    struct Log {
        Log()
            : level_(0)
            , size_(0)
            , autoFlush_(0)
        {
            memset(path_, 0, 50);
        }
        char path_[50];
        unsigned short level_;
        unsigned short size_;
        unsigned short autoFlush_;
    };

    struct Common {
        Common()
            : threadNum_(0)
        {
        }
        unsigned int threadNum_;
    };

    class Info {
    public:
        Info();
        ~Info();
        void GetInfoFromFile(const std::string& fileName);
        const NetWork GetNetWorkInfo() const;
        const DataBase GetDataBaseInfo() const;
        const Log GetLogInfo() const;
        const Common GetCommonInfo() const;

    private:
        NetWork netWork_;
        DataBase dataBase_;
        Log log_;
        Common common_;
    };

}
#endif // !__MY_INFO_H__


