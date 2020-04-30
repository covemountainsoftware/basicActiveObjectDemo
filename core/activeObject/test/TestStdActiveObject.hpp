//
// Created by covemtnsw on 4/25/20.
//

#ifndef ACTIVEOBJECTUNITTESTINGDEMO_TESTSTDACTIVEOBJECT_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_TESTSTDACTIVEOBJECT_HPP

#include "cmsStdActiveObject.hpp"
#include "cmsBaseEvent.hpp"

using TestEvent = cms::BaseEvent<uint32_t>;

enum TestSignals
{
    SIG_T1 = cms::StandardSignals::SM_BEGIN_USER_SIGNALS,
    SIG_T2,
};

class TestStdActiveObject final : public cms::StdActiveObject<TestEvent, 4>
{
public:
    TestStdActiveObject() = default;
    ~TestStdActiveObject() override;
    TestStdActiveObject(const TestStdActiveObject&) = delete;
    TestStdActiveObject& operator=(const TestStdActiveObject&) = delete;
    TestStdActiveObject(TestStdActiveObject&&) = delete;
    TestStdActiveObject& operator=(TestStdActiveObject&&) = delete;

private:
    StateRtn InitialPseudoState(const TestEvent* const event) override;
    StateRtn TestStateA(const TestEvent* const event);
    StateRtn TestStateB(const TestEvent* const event);
};

#endif //ACTIVEOBJECTUNITTESTINGDEMO_TESTSTDACTIVEOBJECT_HPP
