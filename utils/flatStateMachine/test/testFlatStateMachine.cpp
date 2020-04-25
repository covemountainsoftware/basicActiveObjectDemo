/*
MIT License

Copyright (c) <2019-2020> <Matthew Eshleman - https://covemountainsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "testFlatStateMachine.hpp"
#include "CppUTestExt/MockSupport.h"

using namespace CMS;

TestFlatStateMachine::StateRtn TestFlatStateMachine::InitialPseudoState(const TestEvent* const)
{
    mock("TestFlatStateMachine").actualCall("InitialPseudoState").onObject(this);
    return TransitionTo(&TestFlatStateMachine::TestStateA);
}

TestFlatStateMachine::StateRtn TestFlatStateMachine::TestStateA(const TestEvent* const event)
{
    StateRtn rtn;

    switch (event->signal)
    {
    case SM_ENTER:
        mock("TestFlatStateMachine").actualCall("A-ENTER").onObject(this);
        rtn = Handled();
        break;
    case SM_EXIT:
        mock("TestFlatStateMachine").actualCall("A-EXIT").onObject(this);
        rtn = Handled();
        break;

    case SIG_T1:
        mock("TestFlatStateMachine").actualCall("A-T1-HANDLED").onObject(this);
        rtn = Handled();
        break;
    case SIG_T2:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateB);
        break;
    case SIG_T3:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateC);
        break;
    default:
        mock("TestFlatStateMachine").actualCall("A-Default-HANDLED").onObject(this);
        rtn = Handled();
        break;
    }
    return rtn;
}

TestFlatStateMachine::StateRtn TestFlatStateMachine::TestStateB(const TestEvent* const event)
{
    StateRtn rtn;

    switch (event->signal)
    {
    case SM_ENTER:
        mock("TestFlatStateMachine").actualCall("B-ENTER").onObject(this);
        rtn = Handled();
        break;
    case SM_EXIT:
        mock("TestFlatStateMachine").actualCall("B-EXIT").onObject(this);
        rtn = Handled();
        break;

    case SIG_T1:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateA);
        break;
    case SIG_T2:
        mock("TestFlatStateMachine").actualCall("B-T2-HANDLED").onObject(this);
        rtn = Handled();
        break;
    case SIG_T3:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateC);
        break;
    default:
        mock("TestFlatStateMachine").actualCall("B-Default-HANDLED").onObject(this);
        rtn = Handled();
        break;
    }
    return rtn;
}

TestFlatStateMachine::StateRtn TestFlatStateMachine::TestStateC(const TestEvent* const event)
{
    StateRtn rtn;

    switch (event->signal)
    {
    case SM_ENTER:
        mock("TestFlatStateMachine").actualCall("C-ENTER").onObject(this);
        rtn = Handled();
        break;
    case SM_EXIT:
        mock("TestFlatStateMachine").actualCall("C-EXIT").onObject(this);
        rtn = Handled();
        break;

    case SIG_T1:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateA);
        break;
    case SIG_T2:
        rtn = TransitionTo(&TestFlatStateMachine::TestStateB);
        break;
    case SIG_T3:
        mock("TestFlatStateMachine").actualCall("C-T3-HANDLED").onObject(this);
        rtn = Handled();
        break;
    default:
        mock("TestFlatStateMachine").actualCall("C-Default-HANDLED").onObject(this);
        rtn = Handled();
        break;
    }

    return rtn;
}
