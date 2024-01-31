#ifndef __MY_INFO_H__
#define __MY_INFO_H__

#include <string>

namespace cwy {
    struct NetWorkInfo {
        NetWorkInfo()
            : tcpPort_(0)
            , udpPort_(0)
        {
            memset(ip_, 0, 50);
        }
        char ip_[50];
        unsigned int tcpPort_;
        unsigned int udpPort_;
    };

    struct DataBaseInfo {
        DataBaseInfo()
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

    struct LogInfo {
        LogInfo()
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

    struct CommonInfo {
        CommonInfo()
            : threadNum_(0)
        {
            memset(file_address_, 0, 100);
        }
        unsigned int threadNum_;
        char file_address_[100];
    };

    class Info {
    public:
        Info();
        ~Info();
        void GetInfoFromFile(const std::string& fileName);
        const NetWorkInfo GetNetWorkInfo() const;
        const DataBaseInfo GetDataBaseInfo() const;
        const LogInfo GetLogInfo() const;
        const CommonInfo GetCommonInfo() const;

    private:
        NetWorkInfo netWork_;
        DataBaseInfo dataBase_;
        LogInfo log_;
        CommonInfo common_;
    };

}
#endif // !__MY_INFO_H__


