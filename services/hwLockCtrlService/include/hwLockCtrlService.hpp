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

#ifndef ACTIVEOBJECTUNITTESTINGDEMO_HWLOCKCTRLSERVICE_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_HWLOCKCTRLSERVICE_HPP

#include "cmsStdActiveObject.hpp"
#include "servicesActiveObjectType.hpp"
#include <atomic>

namespace cms
{

/**
 * @brief the HwLockCtrlService provides for higher level hardware lock control behavior.
 *        For example, this service will automatically return the hardware lock to its last
 *        state after completing a self test request.
 *
 * @note: This service "is a" active object, which in turn "is a" flat state machine,
 *        so tests and usage of this service may apply to nearly any module
 *        with an internal event driven state machine.
 */
class HwLockCtrlService : public ServicesActiveObject
{
public:
    enum class LockState
    {
        UNKNOWN,
        LOCKED,
        UNLOCKED
    };

    enum class SelfTestResult
    {
        PASS,
        FAIL
    };

    using ChangeStateCallback = std::function<void(LockState)>; //again, normally avoid std::function in firmware.
    using SelfTestResultCallback = std::function<void(SelfTestResult)>;

    HwLockCtrlService() = default;
    virtual ~HwLockCtrlService() = default;
    HwLockCtrlService(const HwLockCtrlService&) = delete;
    HwLockCtrlService& operator=(const HwLockCtrlService&) = delete;
    HwLockCtrlService(HwLockCtrlService&&) = delete;
    HwLockCtrlService& operator=(HwLockCtrlService&&) = delete;

    LockState GetState() const { return mState; }
    void RegisterChangeStateCallback(ChangeStateCallback callback);
    void RegisterSelfTestResultCallback(SelfTestResultCallback callback);

    void RequestLockedAsync();
    void RequestUnlockedAsync();
    void RequestSelfTestAsync();

protected:
    enum Signals
    {
        REQUEST_LOCKED = cms::StandardSignals::SM_BEGIN_USER_SIGNALS,
        REQUEST_UNLOCKED,
        REQUEST_SELF_TEST
    };

    StateRtn InitialPseudoState(const ServiceEventType* const event) override;
    StateRtn Locked(const ServiceEventType* const event);
    StateRtn Unlocked(const ServiceEventType* const event);
    StateRtn SelfTest(const ServiceEventType* const event);

private:
    void NotifyChangedState(LockState state);
    void NotifySelfTestResult(SelfTestResult result);
    void PerformSelfTest();

    using StateFunc = StateRtn (HwLockCtrlService::*)(const ServiceEventType* const);
    std::atomic<LockState> mState {LockState::UNKNOWN };
    ChangeStateCallback mChangedCallback = nullptr;
    SelfTestResultCallback mSelfTestResultCallback = nullptr;
    StateFunc mHistory = nullptr;
};

} // namespace cms

#endif //ACTIVEOBJECTUNITTESTINGDEMO_HWLOCKCTRLSERVICE_HPP
