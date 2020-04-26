//
// Created by covemtnsw on 4/25/20.
//

#include "TestStdActiveObject.hpp"
#include "CppUTestExt/MockSupport.h"

TestStdActiveObject::StateRtn TestStdActiveObject::InitialPseudoState(const TestEvent* const )
{
    mock("TestStdActiveObject").actualCall("InitialPseudoState").onObject(this);
    return TransitionTo(&TestStdActiveObject::TestStateA);
}

TestStdActiveObject::StateRtn TestStdActiveObject::TestStateA(const TestEvent* const event)
{
    StateRtn rtn;

    switch (event->signal)
    {
    case cms::SM_ENTER:
        mock("TestStdActiveObject").actualCall("A-ENTER").onObject(this);
        rtn = Handled();
        break;
    case cms::SM_EXIT:
        mock("TestStdActiveObject").actualCall("A-EXIT").onObject(this);
        rtn = Handled();
        break;

    case SIG_T1:
        mock("TestStdActiveObject").actualCall("A-T1-HANDLED").onObject(this);
        rtn = Handled();
        break;
    case SIG_T2:
        rtn = TransitionTo(&TestStdActiveObject::TestStateB);
        break;

    default:
        mock("TestStdActiveObject").actualCall("A-Default-HANDLED").onObject(this);
        rtn = Handled();
        break;
    }
    return rtn;
}

TestStdActiveObject::StateRtn TestStdActiveObject::TestStateB(const TestEvent* const event)
{
    StateRtn rtn;

    switch (event->signal)
    {
    case cms::SM_ENTER:
        mock("TestStdActiveObject").actualCall("B-ENTER").onObject(this);
        rtn = Handled();
        break;
    case cms::SM_EXIT:
        mock("TestStdActiveObject").actualCall("B-EXIT").onObject(this);
        rtn = Handled();
        break;

    case SIG_T1:
        rtn = TransitionTo(&TestStdActiveObject::TestStateA);
        break;
    case SIG_T2:
        mock("TestStdActiveObject").actualCall("B-T2-HANDLED").onObject(this);
        rtn = Handled();
        break;
    default:
        mock("TestStdActiveObject").actualCall("B-Default-HANDLED").onObject(this);
        rtn = Handled();
        break;
    }
    return rtn;
}
