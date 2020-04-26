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

#include "cmsFlatStateMachine.hpp"
#include "lockCtrlService.hpp"
#include "lockCtrl.h"

namespace cms
{

void LockCtrlService::RegisterChangeStateCallback(LockCtrlService::ChangeStateCallback callback)
{
    //note, this is not thread safe, but
    //assuming this is a single one time call/setup
    //during system start up, so skipping mutex protection.
    mChangedCallback = callback;
}

void LockCtrlService::RegisterSelfTestResultCallback(LockCtrlService::SelfTestResultCallback callback)
{
    //note, this is not thread safe, but
    //assuming this is a single one time call/setup
    //during system start up, so skipping mutex protection.
    mSelfTestResultCallback = callback;
}

void LockCtrlService::RequestLockedAsync()
{
    this->Post(Signals::REQUEST_LOCKED);
}

void LockCtrlService::RequestUnlockedAsync()
{
    this->Post(Signals::REQUEST_UNLOCKED);
}

void LockCtrlService::RequestSelfTestAsync()
{
    this->Post(Signals::REQUEST_SELF_TEST);
}

LockCtrlService::StateRtn LockCtrlService::InitialPseudoState(const ServiceEventType* const)
{
    LockCtrlInit();
    return TransitionTo(&LockCtrlService::Locked);
}

LockCtrlService::StateRtn LockCtrlService::Locked(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        LockCtrlLock();
        NotifyChangedState(LockState::LOCKED);
        rtn = Handled();
        break;
    case cms::SM_EXIT:
        rtn = Handled();
        mHistory = &LockCtrlService::Locked;
        break;
    case REQUEST_LOCKED:
        rtn = Handled();
        break;
    case REQUEST_UNLOCKED:
        rtn = TransitionTo(&LockCtrlService::Unlocked);
        break;
    case REQUEST_SELF_TEST:
        rtn = TransitionTo(&LockCtrlService::SelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

LockCtrlService::StateRtn LockCtrlService::Unlocked(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        rtn = Handled();
        LockCtrlUnlock();
        NotifyChangedState(LockState::UNLOCKED);
        break;
    case cms::SM_EXIT:
        rtn = Handled();
        mHistory = &LockCtrlService::Unlocked;
        break;
    case REQUEST_LOCKED:
        rtn = TransitionTo(&LockCtrlService::Locked);
        break;
    case REQUEST_UNLOCKED:
        rtn = Handled();
        break;
    case REQUEST_SELF_TEST:
        rtn = TransitionTo(&LockCtrlService::SelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

LockCtrlService::StateRtn LockCtrlService::SelfTest(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        PerformSelfTest();
        rtn = Handled();
        break;
    case REQUEST_LOCKED:
        rtn = TransitionTo(&LockCtrlService::Locked);
        break;
    case REQUEST_UNLOCKED:
        rtn = TransitionTo(&LockCtrlService::Unlocked);
        break;
    case REQUEST_SELF_TEST:
        rtn = Handled();
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}
void LockCtrlService::PerformSelfTest()
{
    LockCtrlSelfTestResult result;
    bool ok = LockCtrlSelfTest(&result);
    if (ok && (result == LOCK_CTRL_SELF_TEST_PASSED))
    {
        NotifySelfTestResult(SelfTestResult::PASS);
    }
    else
    {
        NotifySelfTestResult(SelfTestResult::FAIL);
    }

    //remind self to transition back to
    //history per this service's requirements
    //note the use of "PostUrgent" as per:
    //
    // https://covemountainsoftware.com/2020/03/08/uml-statechart-handling-errors-when-entering-a-state/
    //
    if (mHistory == &LockCtrlService::Unlocked)
    {
        PostUrgent(REQUEST_UNLOCKED);
    }
    else
    {
        PostUrgent(REQUEST_LOCKED);
    }
}

void LockCtrlService::NotifyChangedState(LockCtrlService::LockState state)
{
    mState = state;
    if (mChangedCallback)
    {
        mChangedCallback(mState);
    }
}

void LockCtrlService::NotifySelfTestResult(LockCtrlService::SelfTestResult result)
{
    if (mSelfTestResultCallback)
    {
        mSelfTestResultCallback(result);
    }
}

} // namespace cms