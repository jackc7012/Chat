#ifndef __NETWORKHANDLE_H__
#define __NETWORKHANDLE_H__

#include "public.h"
#include "data_base.h"

namespace cwy {
class NetWorkHandle {
public:
    NetWorkHandle();

    int InitDataBase(const std::string& ip, const std::string& name);

    std::string HandleLogin(const s_HandleRecv& handleRecv);

private:
    std::unique_ptr<DataBase> dataBase;
};
}

#endif //!__NETWORKHANDLE_H__