#ifndef __NET_WORK_EVENT_H__
#define __NET_WORK_EVENT_H__

#include "public.h"
#include "data_base.h"

namespace cwy {
class NetWorkEvent
{
public:
    ~NetWorkEvent();

    int InitDataBase(const std::string& ip, const std::string& name);

    void NetWorkEventHandle(const s_HandleRecv& taskContent, int& code, std::string& msg);

private:
    void NetWorkRegisterHandle(const s_HandleRecv& taskContent, int code, std::string& msg);

private:
    DataBase *dataBase{nullptr};
};

}
#endif // !__NET_WORK_EVENT_H__