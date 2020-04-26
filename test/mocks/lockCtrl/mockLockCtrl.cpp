#include "lockCtrl.h"
#include "CppUTestExt/MockSupport.h"

static constexpr const char * MOCK_NAME = "LockCtrl";

bool LockCtrlInit()
{
    mock(MOCK_NAME).actualCall("Init");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true)); //use IntValue due to bug in CppUTest bool handling.
}

bool LockCtrlLock()
{
    mock(MOCK_NAME).actualCall("Lock");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}

bool LockCtrlUnlock()
{
    mock(MOCK_NAME).actualCall("Unlock");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}

bool LockCtrlSelfTest(LockCtrlSelfTestResult* outResult)
{
    mock(MOCK_NAME).actualCall("SelfTest").withOutputParameter("outResult", outResult);
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}
