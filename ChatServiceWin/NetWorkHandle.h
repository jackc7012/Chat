#ifndef __NETWORKHANDLE_H__
#define __NETWORKHANDLE_H__

#include "public.h"
#include "data_base.h"

namespace cwy {
class NetWorkHandle {
public:
    NetWorkHandle();

    int InitDataBase(const std::string& ip, const std::string& name);

    std::string HandleRegister(const s_HandleRecv& handleRecv, const std::string& ip, s_HandleRecv& handleSend);

    std::string HandleLogin(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend, bool& isLoginSucceed);

    void HandleExit(const std::string& customer);

    std::string HandleChat(const s_HandleRecv& handleRecv, s_HandleRecv& handleSend);

private:
    std::unique_ptr<DataBase> dataBase;
};
}

#endif //!__NETWORKHANDLE_H__