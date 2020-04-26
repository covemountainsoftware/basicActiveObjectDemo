//
// Created by covemtnsw on 4/26/20.
//

#ifndef ACTIVEOBJECTUNITTESTINGDEMO_SERVICESACTIVEOBJECTTYPE_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_SERVICESACTIVEOBJECTTYPE_HPP

#include "cmsStdActiveObject.hpp"
#include "servicesCommonEventType.hpp"

namespace cms
{
static constexpr uint32_t SERVICES_EVENT_QUEUE_DEPTH = 10;
using ServicesActiveObject = StdActiveObject<ServiceEventType, SERVICES_EVENT_QUEUE_DEPTH>;

} // namespace cms

#endif //ACTIVEOBJECTUNITTESTINGDEMO_SERVICESACTIVEOBJECTTYPE_HPP
