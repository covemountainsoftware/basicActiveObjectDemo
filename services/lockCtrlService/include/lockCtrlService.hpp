
#ifndef ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRLSERVICE_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRLSERVICE_HPP

#include "cmsStdActiveObject.hpp"
#include "servicesActiveObjectType.hpp"
#include <atomic>

namespace cms
{

/**
 * @brief the LockCtrlService provides for higher level hardware lock control behavior.
 *        For example, this service will automatically return the lock to its last
 *        state after completing a self test request.
 *
 * @note: This service "is a" active object, which in turn "is a" flat state machine,
 *        so tests and usage of this service apply to nearly any module
 *        with an internal event driven state machine.
 */
class LockCtrlService : public ServicesActiveObject
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

    LockCtrlService() = default;
    virtual ~LockCtrlService() = default;

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

    using StateFunc = StateRtn (LockCtrlService::*)(const ServiceEventType* const);
    std::atomic<LockState> mState{ LockState::UNKNOWN };
    ChangeStateCallback mChangedCallback = nullptr;
    SelfTestResultCallback mSelfTestResultCallback = nullptr;
    StateFunc mHistory = nullptr;
};

} // namespace cms

#endif //ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRLSERVICE_HPP
