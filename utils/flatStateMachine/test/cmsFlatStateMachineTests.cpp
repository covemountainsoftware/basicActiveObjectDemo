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
#include "testFlatStateMachine.hpp"

static const TestEvent TestEventT1(SIG_T1);
static const TestEvent TestEventT2(SIG_T2);
static const TestEvent TestEventT3(SIG_T3);

TEST_GROUP(FlatStateMachineTests)
{
    TestFlatStateMachine* mUnderTest;

    void setup()
    {
        mUnderTest = new TestFlatStateMachine();
    }

    void teardown()
    {
        delete mUnderTest;
        mock().clear();
    }

    void Init()
    {
        mUnderTest->Initialize();
    }

    void InitToA()
    {
        mock("TestFlatStateMachine").expectOneCall("InitialPseudoState").onObject(mUnderTest);
        mock("TestFlatStateMachine").expectOneCall("A-ENTER").onObject(mUnderTest);
        Init();
        mock().checkExpectations();
    }

    void InitToB()
    {
        InitToA();
        mock("TestFlatStateMachine").expectOneCall("A-EXIT").onObject(mUnderTest);
        mock("TestFlatStateMachine").expectOneCall("B-ENTER").onObject(mUnderTest);
        mUnderTest->ProcessEvent(&TestEventT2);
        mock().checkExpectations();
    }

    void InitToC()
    {
        InitToA();
        mock("TestFlatStateMachine").expectOneCall("A-EXIT").onObject(mUnderTest);
        mock("TestFlatStateMachine").expectOneCall("C-ENTER").onObject(mUnderTest);
        mUnderTest->ProcessEvent(&TestEventT3);
        mock().checkExpectations();
    }
};

TEST(FlatStateMachineTests, create_does_not_crash)
{
    mock("TestFlatStateMachine").ignoreOtherCalls();
    Init();
}

TEST(FlatStateMachineTests, init_behaves_as_expected)
{
    InitToA();
}

TEST(FlatStateMachineTests, a_handled_event)
{
    InitToA();
    mock("TestFlatStateMachine").expectOneCall("A-T1-HANDLED").onObject(mUnderTest);
    mUnderTest->ProcessEvent(&TestEventT1);
    mock().checkExpectations();
}

TEST(FlatStateMachineTests, tran_to_a_to_b_behaves_as_expected)
{
    InitToB();
}

TEST(FlatStateMachineTests, b_handled_event)
{
    InitToB();
    mock("TestFlatStateMachine").expectOneCall("B-T2-HANDLED").onObject(mUnderTest);
    mUnderTest->ProcessEvent(&TestEventT2);
    mock().checkExpectations();
}

TEST(FlatStateMachineTests, tran_to_a_to_c_behaves_as_expected)
{
    InitToC();
}

TEST(FlatStateMachineTests, c_handled_event)
{
    InitToC();
    mock("TestFlatStateMachine").expectOneCall("C-T3-HANDLED").onObject(mUnderTest);
    mUnderTest->ProcessEvent(&TestEventT3);
    mock().checkExpectations();
}
