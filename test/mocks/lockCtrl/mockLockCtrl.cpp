#include "lockCtrl.h"
#include <cstdio>
#include "CppUTestExt/MockSupport.h"

bool LockCtrlInit()
{
    mock("LockCtrl").actualCall("Init");
    return static_cast<bool>(mock("LockCtrl").returnIntValueOrDefault(true)); //use IntValue due to bug in CppUTest bool handling.
}

bool LockCtrlLock()
{
    mock("LockCtrl").actualCall("Lock");
    return static_cast<bool>(mock("LockCtrl").returnIntValueOrDefault(true));
}

bool LockCtrlUnlock()
{
    mock("LockCtrl").actualCall("Unlock");
    return static_cast<bool>(mock("LockCtrl").returnIntValueOrDefault(true));
}

bool LockCtrlSelfTest(LockCtrlSelfTestResult* outResult)
{
    mock("LockCtrl").actualCall("SelfTest").withOutputParameter("outResult", outResult);
    return static_cast<bool>(mock("LockCtrl").returnIntValueOrDefault(true));
}
