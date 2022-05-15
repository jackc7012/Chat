#ifndef __NET_WORK_EVENT_H__
#define __NET_WORK_EVENT_H__

#include "public.h"
#include "CDataBase.h"

namespace cwy {
class NetWorkEvent
{
public:
    ~NetWorkEvent();

    int InitDataBase(const std::string& ip, const std::string& name);

    void NetWorkEventHandle(const s_HandleRecv& taskContent, CommunicationType& type, std::string& msg);

private:
    void NetWorkRegisterHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend);

    void NetWorkLoginHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend);

    void NetWorkDelHandle(const s_HandleRecv& taskContent);

private:
    std::unique_ptr<DataBaseImpl> dataBase{nullptr};
};

}
#endif // !__NET_WORK_EVENT_H__