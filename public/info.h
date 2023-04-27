#ifndef MY_INFO_H
#define MY_INFO_H

#include <string>

namespace cwy {
    struct NetWork {
        char ip_[50];
        unsigned int tcpPort_;
        unsigned int udpPort_;
    };

    struct DataBase {
        char ip_[50];
        char name_[50];
        char uid_[50];
        char pwd_[50];
    };

    struct Common {
        unsigned int threadNum_;
    };

    class Info
    {
    public:
        Info();
        ~Info();
        void GetInfoFromFile(const std::string& fileName);
        const NetWork GetNetWorkInfo() const;
        const DataBase GetDataBaseInfo() const;
        const Common GetCommonInfo() const;

    private:
        NetWork netWork_;
        DataBase dataBase_;
        Common common_;
    };

}
#endif // !MY_INFO_H


