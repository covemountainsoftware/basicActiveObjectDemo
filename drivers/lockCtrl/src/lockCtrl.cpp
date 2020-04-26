/*
 *   This concrete implementation of the LockCtrl "C" style interface
 *   is for demo purposes only. The focus of this project
 *   is how to test state machines and active objects, hence
 *   this fake hardware driver module.
 */
#include "lockCtrl.h"
#include <cstdio>

bool LockCtrlInit()
{
    printf("%s() executed\n", __FUNCTION__ );
    return true;
}

bool LockCtrlLock()
{
    printf("%s() executed\n", __FUNCTION__ );
    return true;
}

bool LockCtrlUnlock()
{
    printf("%s() executed\n", __FUNCTION__ );
    return true;
}

bool LockCtrlSelfTest(LockCtrlSelfTestResult* outResult)
{
    printf("%s() executed\n", __FUNCTION__ );
    if (outResult)
    {
        *outResult = LOCK_CTRL_SELF_TEST_PASSED;
        return true;
    }
    else
    {
        printf("%s() executed with nullptr arg\n", __FUNCTION__ );
        return false;
    }
}
