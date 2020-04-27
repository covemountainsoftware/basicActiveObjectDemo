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

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestStdActiveObject.hpp"

static const TestEvent TestEventT1(SIG_T1);
static const TestEvent TestEventT2(SIG_T2);

/**
 * @brief This test is testing portions of the active object class,
 *        but does NOT test actual thread behavior, on purpose, as we
 *        want to avoid real thread behavior testing in our unit tests.
 *
 *        This would be especially true for firmware/embedded software
 *        unit testing, where threads would likely behave in a different
 *        manner when compared to standard PC threads.
 */
TEST_GROUP(TestStdActiveObjectTests)
{
    TestStdActiveObject* mUnderTest;

    void setup()
    {
        mUnderTest = new TestStdActiveObject();
    }

    void teardown()
    {
        delete mUnderTest;
        mock().clear();
    }

    void Start()
    {
        mUnderTest->Start(cms::ProcessOption::UNIT_TEST);
    }

    /**
     * @brief effectively allow the Active Object under test
     *        CPU time to process all events in the internal queue
     */
    void ActiveObjectProcess()
    {
        while (true == mUnderTest->ProcessOneEvent(cms::ProcessOption::UNIT_TEST))
        {
        }
    }

    void InitToA()
    {
        mock("TestStdActiveObject").expectOneCall("InitialPseudoState").onObject(mUnderTest);
        mock("TestStdActiveObject").expectOneCall("A-ENTER").onObject(mUnderTest);
        Start();
        ActiveObjectProcess();
        mock().checkExpectations();
    }

    void InitToB()
    {
        InitToA();
        mock("TestStdActiveObject").expectOneCall("A-EXIT").onObject(mUnderTest);
        mock("TestStdActiveObject").expectOneCall("B-ENTER").onObject(mUnderTest);
        mUnderTest->Post(TestEventT2);
        ActiveObjectProcess();
        mock().checkExpectations();
    }
};

TEST(TestStdActiveObjectTests, create_does_not_crash)
{
    mock("TestStdActiveObject").ignoreOtherCalls();
    Start();
}

TEST(TestStdActiveObjectTests, init_behaves_as_expected)
{
    InitToA();
}

TEST(TestStdActiveObjectTests, processoneevent_returns_false_when_no_events)
{
    InitToA();
    CHECK_FALSE(mUnderTest->ProcessOneEvent(cms::ProcessOption::UNIT_TEST));
}

TEST(TestStdActiveObjectTests, processoneevent_returns_true_when_event_was_processed)
{
    InitToA();
    mock().ignoreOtherCalls(); //this test is only interested in the return of ProcessOneEvent().
    mUnderTest->Post(TestEventT1);
    //first call should return true, as an event is in the queue
    CHECK_TRUE(mUnderTest->ProcessOneEvent(cms::ProcessOption::UNIT_TEST));

    //while here, second call should return false, as there are no more events in the queue
    CHECK_FALSE(mUnderTest->ProcessOneEvent(cms::ProcessOption::UNIT_TEST));
}

TEST(TestStdActiveObjectTests, a_handled_event)
{
    InitToA();
    mock("TestStdActiveObject").expectOneCall("A-T1-HANDLED").onObject(mUnderTest);
    mUnderTest->Post(TestEventT1);
    ActiveObjectProcess();
    mock().checkExpectations();
}

TEST(TestStdActiveObjectTests, post_urgent_operates_as_expected)
{
    InitToA();

    mock("TestStdActiveObject").expectOneCall("A-EXIT").onObject(mUnderTest);
    mock("TestStdActiveObject").expectOneCall("B-ENTER").onObject(mUnderTest);

    mock("TestStdActiveObject").expectOneCall("B-EXIT").onObject(mUnderTest);
    mock("TestStdActiveObject").expectOneCall("A-ENTER").onObject(mUnderTest);

    mUnderTest->Post(TestEventT1);
    mUnderTest->PostUrgent(TestEventT2); //will be processed first, resulting in transition to B
    ActiveObjectProcess();
    mock().checkExpectations();
}

TEST(TestStdActiveObjectTests, tran_a_to_b_behaves_as_expected)
{
    InitToB();
}

TEST(TestStdActiveObjectTests, b_handles_event)
{
    InitToB();
    mock("TestStdActiveObject").expectOneCall("B-T2-HANDLED").onObject(mUnderTest);
    mUnderTest->Post(TestEventT2);
    ActiveObjectProcess();
    mock().checkExpectations();
}

TEST(TestStdActiveObjectTests, returns_false_if_too_many_posts)
{
    InitToA();
    bool ok = mUnderTest->Post(TestEventT2);
    CHECK_EQUAL(true, ok);
    ok = mUnderTest->Post(TestEventT2);
    CHECK_EQUAL(true, ok);
    ok = mUnderTest->Post(TestEventT2);
    CHECK_EQUAL(true, ok);
    ok = mUnderTest->Post(TestEventT2);
    CHECK_EQUAL(true, ok);
    ok = mUnderTest->Post(TestEventT2); //this post should exceed this test AO's max queue depth, as we have not allowed for any processing
    CHECK_EQUAL(false, ok);

    //allow processing time to clear the queue
    mock("TestStdActiveObject").ignoreOtherCalls();
    ActiveObjectProcess();

    //confirm we can again post an event
    ok = mUnderTest->Post(TestEventT2);
    CHECK_EQUAL(true, ok);
}
