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
#include "hwLockCtrlService.hpp"
#include "hwLockCtrl.h"

namespace cms
{

void HwLockCtrlService::RegisterChangeStateCallback(HwLockCtrlService::ChangeStateCallback callback)
{
    //note, this is not thread safe, but
    //assuming this is a single one time call/setup
    //during system start up, so skipping mutex protection.
    mChangedCallback = callback;
}

void HwLockCtrlService::RegisterSelfTestResultCallback(HwLockCtrlService::SelfTestResultCallback callback)
{
    //note, this is not thread safe, but
    //assuming this is a single one time call/setup
    //during system start up, so skipping mutex protection.
    mSelfTestResultCallback = callback;
}

void HwLockCtrlService::RequestLockedAsync()
{
    this->Post(Signals::REQUEST_LOCKED);
}

void HwLockCtrlService::RequestUnlockedAsync()
{
    this->Post(Signals::REQUEST_UNLOCKED);
}

void HwLockCtrlService::RequestSelfTestAsync()
{
    this->Post(Signals::REQUEST_SELF_TEST);
}

HwLockCtrlService::StateRtn HwLockCtrlService::InitialPseudoState(const ServiceEventType* const)
{
    HwLockCtrlInit();
    return TransitionTo(&HwLockCtrlService::Locked);
}

HwLockCtrlService::StateRtn HwLockCtrlService::Locked(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        HwLockCtrlLock();
        NotifyChangedState(LockState::LOCKED);
        rtn = Handled();
        break;
    case cms::SM_EXIT:
        rtn = Handled();
        mHistory = &HwLockCtrlService::Locked;
        break;
    case REQUEST_LOCKED:
        rtn = Handled();
        break;
    case REQUEST_UNLOCKED:
        rtn = TransitionTo(&HwLockCtrlService::Unlocked);
        break;
    case REQUEST_SELF_TEST:
        rtn = TransitionTo(&HwLockCtrlService::SelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

HwLockCtrlService::StateRtn HwLockCtrlService::Unlocked(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        rtn = Handled();
        HwLockCtrlUnlock();
        NotifyChangedState(LockState::UNLOCKED);
        break;
    case cms::SM_EXIT:
        rtn = Handled();
        mHistory = &HwLockCtrlService::Unlocked;
        break;
    case REQUEST_LOCKED:
        rtn = TransitionTo(&HwLockCtrlService::Locked);
        break;
    case REQUEST_UNLOCKED:
        rtn = Handled();
        break;
    case REQUEST_SELF_TEST:
        rtn = TransitionTo(&HwLockCtrlService::SelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

HwLockCtrlService::StateRtn HwLockCtrlService::SelfTest(const ServiceEventType* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case cms::SM_ENTER:
        PerformSelfTest();
        rtn = Handled();
        break;
    case REQUEST_LOCKED:
        rtn = TransitionTo(&HwLockCtrlService::Locked);
        break;
    case REQUEST_UNLOCKED:
        rtn = TransitionTo(&HwLockCtrlService::Unlocked);
        break;

    case REQUEST_SELF_TEST: //purposeful fallthrough
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}
void HwLockCtrlService::PerformSelfTest()
{
    HwLockCtrlSelfTestResult result;
    bool ok = HwLockCtrlSelfTest(&result);
    if (ok && (result == HW_LOCK_CTRL_SELF_TEST_PASSED))
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
    if (mHistory == &HwLockCtrlService::Unlocked)
    {
        PostUrgent(REQUEST_UNLOCKED);
    }
    else
    {
        PostUrgent(REQUEST_LOCKED);
    }
}

void HwLockCtrlService::NotifyChangedState(HwLockCtrlService::LockState state)
{
    mState = state;
    if (mChangedCallback)
    {
        mChangedCallback(mState);
    }
}

void HwLockCtrlService::NotifySelfTestResult(HwLockCtrlService::SelfTestResult result)
{
    if (mSelfTestResultCallback)
    {
        mSelfTestResultCallback(result);
    }
}

} // namespace cms