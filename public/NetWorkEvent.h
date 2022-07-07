#ifndef __NET_WORK_EVENT_H__
#define __NET_WORK_EVENT_H__

#include "public.h"

#include "CDataBase.h"

namespace cwy {
class NetWorkEvent {
public:
    ~NetWorkEvent();

    int InitDataBase(const std::string& ip, const std::string& name);

    SOCKET NetWorkEventHandle(const s_HandleRecv& taskContent, CommunicationType& type, std::string& msg);

private:
    SOCKET NetWorkRegisterHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend);

    SOCKET NetWorkLoginHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend);

    SOCKET NetWorkDelHandle(const s_HandleRecv& taskContent);

    SOCKET NetWorkChatHandle(const s_HandleRecv& taskContent, s_HandleRecv& handleSend);

private:
    std::unique_ptr<DataBaseImpl> dataBase{nullptr};
};

}
#endif // !__NET_WORK_EVENT_H__