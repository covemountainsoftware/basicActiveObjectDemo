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
#include "hwLockCtrlService.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "hwLockCtrl.h"

using namespace cms;

static constexpr const char* HW_LOCK_CTRL_MOCK = "HwLockCtrl";
static constexpr const char* CB_MOCK = "TestCb";

static void TestLockStateCallback(HwLockCtrlService::LockState state)
{
    mock(CB_MOCK).actualCall("LockStateCallback").withIntParameter("state", static_cast<int>(state));
}

static void TestSelfTestResultCallback(HwLockCtrlService::SelfTestResult result)
{
    mock(CB_MOCK).actualCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(result));
}

/**
 * @brief This test demonstrates the following key points:
 *         1) Does NOT test the thread associated with the active object,
 *            rather, tests the behavior of the active object via a back door
 *            to the active object's event queue.
 *         2) Tests the internal state machine of the active object without
 *            internal knowledge of the state machine. Rather, only
 *            by observing the behavior and associated output/results.
 *         3) Create helper methods to drive the internal "state" to known
 *            starting points.
 *         4) Continue to follow software engineering best practices, such
 *            as adhering to the DRY principle.
 */
TEST_GROUP(HwLockCtrlServiceTests)
{
    HwLockCtrlService* mUnderTest = nullptr;

    void setup() final
    {
        mUnderTest = new HwLockCtrlService();
        mUnderTest->RegisterChangeStateCallback(TestLockStateCallback);
        mUnderTest->RegisterSelfTestResultCallback(TestSelfTestResultCallback);
    }

    void teardown() final
    {
        delete mUnderTest;
        mock().clear();
    }

    void GiveProcessingTime()
    {
        //use our unit testing backdoor to service
        //the active object's internal queue. This avoids threading issues
        //with unit tests, creating 100% predictable unit tests.
        while (true == mUnderTest->ProcessOneEvent(ProcessOption::UNIT_TEST)) {}
    }

    void StartServiceToLocked()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Init");
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::LOCKED));
        mUnderTest->Start(ProcessOption::UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(HwLockCtrlService::LockState::LOCKED == mUnderTest->GetState());
    }

    void TestUnlock()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::UNLOCKED));
        mUnderTest->RequestUnlockedAsync();
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(HwLockCtrlService::LockState::UNLOCKED == mUnderTest->GetState());
    }

    void StartServiceToUnlocked()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Init");
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::LOCKED));
        mUnderTest->Start(ProcessOption::UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        TestUnlock();
    }
};

TEST(HwLockCtrlServiceTests, given_startup_when_created_then_does_not_crash)
{
    //setup() is called by cpputest, which creates our unit under test.
}

TEST(HwLockCtrlServiceTests, given_startup_when_started_then_service_ensures_the_lock_is_locked)
{
    //when originally developed, this test contained all the code
    //in the below helper method. Since this "setup" was needed by
    //other tests, it was extracted into a helper method.
    //this pattern of developing tests, discovering the need for common
    //setup helper methods, takes place throughout coding of these unit tests.
    StartServiceToLocked();
}

TEST(HwLockCtrlServiceTests, given_locked_when_another_lock_request_then_service_is_silent)
{
    StartServiceToLocked();
    mUnderTest->RequestLockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_locked_when_unlock_request_then_service_unlocks_the_driver_and_emits_status_callback)
{
    StartServiceToLocked();
    TestUnlock();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_another_unlock_request_then_service_is_silent)
{
    StartServiceToUnlocked();
    mUnderTest->RequestUnlockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_locked_when_selftest_request_then_service_performs_selftest_emits_results_and_returns_to_locked)
{
    StartServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HwLockCtrlService::SelfTestResult::PASS));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::LOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_selftest_request_then_service_performs_selftest_emits_results_and_returns_to_unlocked)
{
    StartServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HwLockCtrlService::SelfTestResult::PASS));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::UNLOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,given_locked_when_selftest_request_which_fails_then_service_still_returns_to_locked)
{
    StartServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_POWER;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HwLockCtrlService::SelfTestResult::FAIL));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::LOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_selftest_request_which_fails_then_service_still_returns_to_unlocked)
{
    StartServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_MOTOR;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HwLockCtrlService::SelfTestResult::FAIL));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HwLockCtrlService::LockState::UNLOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}