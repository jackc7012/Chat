#ifndef __NET_WORK_FACTORY_H__
#define __NET_WORK_FACTORY_H__

#include "public.h"
#include "NetWorkEvent.h"

namespace cwy {
class NetWorkFactory {
public:
    NetWorkEvent* CreateNetWorkEvent(const std::string& type);
};
}
#endif // !__NET_WORK_FACTORY_H__