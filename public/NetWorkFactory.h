#ifndef __NET_WORK_FACTORY_H__
#define __NET_WORK_FACTORY_H__

#include "NetWorkEvent.h"

namespace cwy {
class NetWorkFactory {
public:
    NetWorkEvent* CreateNetWorkEvent(CommunicationType type);

    ~NetWorkFactory();
};
}
#endif // !__NET_WORK_FACTORY_H__