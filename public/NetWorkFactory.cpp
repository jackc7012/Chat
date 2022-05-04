#include "NetWorkFactory.h"

namespace cwy {
NetWorkEvent* NetWorkFactory::CreateNetWorkEvent(const std::string& type)
{
    NetWorkEvent * networkEvent = nullptr;
    if (type == "register") {
        networkEvent = new Register();
    }
    return networkEvent;
}
}
