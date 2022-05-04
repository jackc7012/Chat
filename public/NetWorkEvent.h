#ifndef __NET_WORK_EVENT_H__
#define __NET_WORK_EVENT_H__

#include "../json/json.h"

namespace cwy {
class NetWorkEvent
{
public:
    virtual void NetWorkEventHandle() = 0;
};

class Register : public NetWorkEvent
{
    void NetWorkEventHandle() override;
};

class Login : public NetWorkEvent {
    void NetWorkEventHandle() override;
};
}
#endif // !__NET_WORK_EVENT_H__