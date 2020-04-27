#include <iostream>
#include "hwLockCtrlService.hpp"
#include <thread>

using namespace cms;
static HwLockCtrlService::LockState s_lastState = HwLockCtrlService::LockState::UNKNOWN;
static HwLockCtrlService s_lockCtrlService;

std::string GetLockState()
{
    switch (s_lastState)
    {
    case HwLockCtrlService::LockState::UNLOCKED:
        return "Unlocked";
    case HwLockCtrlService::LockState::LOCKED:
        return "Locked";
    default:
        return "Unknown";
    }
}

void LockStateChangeCallback(HwLockCtrlService::LockState state)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.
    s_lastState = state;
    std::cout << "Lock state changed: " << GetLockState() << std::endl;
}

void SelfTestResultCallback(HwLockCtrlService::SelfTestResult result)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.

    const char* resultStr = (result == HwLockCtrlService::SelfTestResult::PASS) ? "Pass" : "Fail";
    std::cout << "Self Test result: " << resultStr << std::endl;
}

enum class DesiredAction
{
    EXIT,
    LOCK,
    UNLOCK,
    SELF_TEST
};

DesiredAction GetUserAction()
{
    std::string action;

    std::cout << "Lock state is: " << GetLockState() << std::endl;
    std::cout << "0: Unlock the lock" << std::endl;
    std::cout << "1: Lock the lock" << std::endl;
    std::cout << "2: Self Test the lock" << std::endl;
    std::cout << "3: Exit" << std::endl
              << std::endl;

    std::cin >> action;

    switch (action.front())
    {
    case '0':
        return DesiredAction::UNLOCK;
    case '1':
        return DesiredAction::LOCK;
    case '2':
        return DesiredAction::SELF_TEST;
    default:
        return DesiredAction::EXIT;
    }
}

int main()
{
    s_lockCtrlService.RegisterChangeStateCallback(LockStateChangeCallback);
    s_lockCtrlService.RegisterSelfTestResultCallback(SelfTestResultCallback);
    s_lockCtrlService.Start();

    while (true)
    {
        switch (GetUserAction())
        {
        case DesiredAction::LOCK:
            s_lockCtrlService.RequestLockedAsync();
            break;
        case DesiredAction::UNLOCK:
            s_lockCtrlService.RequestUnlockedAsync();
            break;
        case DesiredAction::SELF_TEST:
            s_lockCtrlService.RequestSelfTestAsync();
            break;
        default:
            return 0;
        }
    }

    return 0;
}
