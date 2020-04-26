#ifndef ACTIVEOBJECTUNITTESTINGDEMO_SERVICESCOMMONEVENTTYPE_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_SERVICESCOMMONEVENTTYPE_HPP

#include "cmsBaseEvent.hpp"

namespace cms
{

struct ServiceEventType : public BaseEvent<uint32_t>
{
    ServiceEventType() = default;
    ServiceEventType(uint32_t sig) : BaseEvent(sig) {}
};

} // namespace cms

#endif //ACTIVEOBJECTUNITTESTINGDEMO_SERVICESCOMMONEVENTTYPE_HPP
