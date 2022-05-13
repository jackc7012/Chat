#include "NetWorkFactory.h"

namespace cwy {
NetWorkEvent* NetWorkFactory::CreateNetWorkEvent(CommunicationType type)
{
    NetWorkEvent *networkEvent = nullptr;
    switch (type) {
        case CommunicationType::REGISTER:
        {
            networkEvent = new Register();
            break;
        }

        case CommunicationType::LOGIN:
        {
            networkEvent = new Login();
            break;
        }

        default:
            break;
    }
    return networkEvent;
}

NetWorkFactory::~NetWorkFactory()
{
}
}
