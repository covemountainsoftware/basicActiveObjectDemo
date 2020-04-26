
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
