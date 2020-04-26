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


/**
 * @brief public header for the Lock Ctrl driver.
 *        This driver controls a physical electronic
 *        controlled lock. The API is fully synchronous.
 */
#ifndef ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRL_H
#define ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LockCtrlSelfTestResult enumerates
 *        possible self test results.
 */
enum LockCtrlSelfTestResult
{
    LOCK_CTRL_SELF_TEST_PASSED,
    LOCK_CTRL_SELF_TEST_FAILED_POWER,
    LOCK_CTRL_SELF_TEST_FAILED_MOTOR,
};

/**
 * @brief LockCtrlInit initializes the driver. Lock state is undefined.
 * @return true - initialization completed successfully.
 *         false - some error.
 */
bool LockCtrlInit();

/**
 * @brief  LockCtrlLock locks the lock.
 * @return true - lock operation completed successfully
 *         false - some error.
 */
bool LockCtrlLock();

/**
 * @brief  LockCtrlUnlock unlocks the lock.
 * @return true - unlock operation completed successfully
 *         false - some error.
 */
bool LockCtrlUnlock();

/**
 * @brief LockCtrlSelfTest executes a self test. When completed, the Lock is always LOCKED.
 * @arg outResult: [out] output the self test results
 * @return true - self test completed and results are available in 'outResult'
 *         false - self test failed to execute.
 */
bool LockCtrlSelfTest(LockCtrlSelfTestResult* outResult);

#ifdef __cplusplus
}
#endif

#endif //ACTIVEOBJECTUNITTESTINGDEMO_LOCKCTRL_H
