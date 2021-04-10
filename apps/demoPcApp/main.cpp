#include <iostream>
#include "hwLockCtrlService.h"
#include <string>

static HLCS_LockStateT s_lastState = HLCS_LOCK_STATE_UNKNOWN;

std::string GetLockState()
{
    switch (s_lastState)
    {
    case HLCS_LOCK_STATE_UNLOCKED:
        return "Unlocked";
    case HLCS_LOCK_STATE_LOCKED:
        return "Locked";
    default:
        return "Unknown";
    }
}

void LockStateChangeCallback(HLCS_LockStateT state)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.
    s_lastState = state;
    std::cout << "Lock state changed: " << GetLockState() << std::endl;
}

void SelfTestResultCallback(HLCS_SelfTestResultT result)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.

    const char* resultStr = (result == HLCS_SELF_TEST_RESULT_PASS) ? "Pass" : "Fail";
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
    HLCS_Init(EXECUTION_OPTION_NORMAL);
    HLCS_RegisterChangeStateCallback(LockStateChangeCallback);
    HLCS_RegisterSelfTestResultCallback(SelfTestResultCallback);
    HLCS_Start();

    while (true)
    {
        switch (GetUserAction())
        {
        case DesiredAction::LOCK:
            HLCS_RequestLockedAsync();
            break;
        case DesiredAction::UNLOCK:
            HLCS_RequestUnlockedAsync();
            break;
        case DesiredAction::SELF_TEST:
            HLCS_RequestSelfTestAsync();
            break;
        default:
            HLCS_Destroy();
            return 0;
        }
    }

    return 0;
}
