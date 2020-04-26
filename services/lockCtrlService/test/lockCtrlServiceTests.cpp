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
#include "lockCtrlService.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "lockCtrl.h"

using namespace cms;

static void TestLockStateCallback(LockCtrlService::LockState state)
{
    mock("Test").actualCall("LockStateCallback").withIntParameter("state", static_cast<int>(state));
}

static void TestSelfTestResultCallback(LockCtrlService::SelfTestResult result)
{
    mock("Test").actualCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(result));
}

/**
 * @brief This test is demonstrates two key points:
 *         1) Does not test the thread associated with the active object,
 *            rather, tests the behavior of the active object via a back door
 *            to the active object's event queue.
 *         2) Tests the internal state machine of the active object without
 *            internal knowledge of the state machine. Rather, only
 *            by observing the results.
 */
TEST_GROUP(LockCtrlServiceTests)
{
    LockCtrlService* mUnderTest;

    void setup()
    {
        mUnderTest = new LockCtrlService();
        mUnderTest->RegisterChangeStateCallback(TestLockStateCallback);
        mUnderTest->RegisterSelfTestResultCallback(TestSelfTestResultCallback);
    }

    void teardown()
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
        mock("LockCtrl").expectOneCall("Init");
        mock("LockCtrl").expectOneCall("Lock");
        mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::LOCKED));
        mUnderTest->Start(ProcessOption::UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(LockCtrlService::LockState::LOCKED == mUnderTest->GetState());
    }

    void TestUnlock()
    {
        mock("LockCtrl").expectOneCall("Unlock");
        mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::UNLOCKED));
        mUnderTest->RequestUnlockedAsync();
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(LockCtrlService::LockState::UNLOCKED == mUnderTest->GetState());
    }

    void StartServiceToUnlocked()
    {
        mock("LockCtrl").expectOneCall("Init");
        mock("LockCtrl").expectOneCall("Lock");
        mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::LOCKED));
        mUnderTest->Start(ProcessOption::UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        TestUnlock();
    }
};

TEST(LockCtrlServiceTests, create_does_not_crash)
{
}

TEST(LockCtrlServiceTests, start_locks_the_lock)
{
    StartServiceToLocked();
}

TEST(LockCtrlServiceTests, when_already_locked_another_lock_request_is_silent)
{
    StartServiceToLocked();
    mUnderTest->RequestLockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(LockCtrlServiceTests, when_locked_unlock_is_processed_as_expected)
{
    StartServiceToLocked();
    TestUnlock();
}

TEST(LockCtrlServiceTests, when_already_unlocked_another_unlock_request_is_silent)
{
    StartServiceToUnlocked();
    mUnderTest->RequestUnlockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(LockCtrlServiceTests, when_locked_self_test_operates_and_returns_to_locked)
{
    StartServiceToLocked();
    mock().clear();

    auto passed = LOCK_CTRL_SELF_TEST_PASSED;
    mock("LockCtrl").expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock("Test").expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(LockCtrlService::SelfTestResult::PASS));
    mock("LockCtrl").expectOneCall("Lock");
    mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::LOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(LockCtrlServiceTests, when_unlocked_self_test_operates_and_returns_to_unlocked)
{
    StartServiceToUnlocked();
    mock().clear();

    auto passed = LOCK_CTRL_SELF_TEST_PASSED;
    mock("LockCtrl").expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock("Test").expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(LockCtrlService::SelfTestResult::PASS));
    mock("LockCtrl").expectOneCall("Unlock");
    mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::UNLOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(LockCtrlServiceTests, when_locked_a_failed_self_test_operates_and_still_returns_to_locked)
{
    StartServiceToLocked();
    mock().clear();

    auto passed = LOCK_CTRL_SELF_TEST_FAILED_POWER;
    mock("LockCtrl").expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock("Test").expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(LockCtrlService::SelfTestResult::FAIL));
    mock("LockCtrl").expectOneCall("Lock");
    mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::LOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(LockCtrlServiceTests, when_unlocked_a_failed_self_test_operates_and_still_returns_to_unlocked)
{
    StartServiceToUnlocked();
    mock().clear();

    auto passed = LOCK_CTRL_SELF_TEST_FAILED_MOTOR;
    mock("LockCtrl").expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock("Test").expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(LockCtrlService::SelfTestResult::FAIL));
    mock("LockCtrl").expectOneCall("Unlock");
    mock("Test").expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(LockCtrlService::LockState::UNLOCKED));
    mUnderTest->RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}