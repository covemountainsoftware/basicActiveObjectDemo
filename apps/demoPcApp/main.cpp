#include <iostream>
#include "lockCtrlService.hpp"
#include <thread>

using namespace cms;
static LockCtrlService::LockState s_lastState = LockCtrlService::LockState::UNKNOWN;
static LockCtrlService s_lockCtrlService;

std::string GetLockState()
{
    switch (s_lastState)
    {
    case LockCtrlService::LockState::UNLOCKED:
        return "Unlocked";
    case LockCtrlService::LockState::LOCKED:
        return "Locked";
    default:
        return "Unknown";
    }
}

void LockStateChangeCallback(LockCtrlService::LockState state)
{
    s_lastState = state;
    std::cout << "Lock state changed: " << GetLockState() << std::endl;
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
    int action;

    std::cout << "Lock state is: " << GetLockState() << std::endl;
    std::cout << "0: Unlock the lock" << std::endl;
    std::cout << "1: Lock the lock" << std::endl;
    std::cout << "2: Self Test the lock" << std::endl;
    std::cout << "3: Exit" << std::endl;

    std::cin >> action;

    switch (action)
    {
    case 0:
        return DesiredAction::UNLOCK;
    case 1:
        return DesiredAction::LOCK;
    case 2:
        return DesiredAction::SELF_TEST;
    default:
        return DesiredAction::EXIT;
    }
}

int main()
{
    s_lockCtrlService.RegisterChangeStateCallback(LockStateChangeCallback);
    s_lockCtrlService.Start();

    DesiredAction desiredAction;
    while (DesiredAction::EXIT != (desiredAction = GetUserAction()))
    {
        switch (desiredAction)
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
            break;
        }
    }

    return 0;
}
