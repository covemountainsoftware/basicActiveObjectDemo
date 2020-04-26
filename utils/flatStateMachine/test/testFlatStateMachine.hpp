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

#ifndef ACTIVEOBJECTUNITTESTINGDEMO_TESTFLATSTATEMACHINE_HPP
#define ACTIVEOBJECTUNITTESTINGDEMO_TESTFLATSTATEMACHINE_HPP

#include "cmsFlatStateMachine.hpp"
#include "testEvent.hpp"

class TestFlatStateMachine : public cms::FlatStateMachine<TestEvent>
{
public:
    TestFlatStateMachine() = default;
    virtual ~TestFlatStateMachine() = default;

private:
    StateRtn InitialPseudoState(const TestEvent* const event) override;

    StateRtn TestStateA(const TestEvent* const event);
    StateRtn TestStateB(const TestEvent* const event);
    StateRtn TestStateC(const TestEvent* const event);
};

#endif //ACTIVEOBJECTUNITTESTINGDEMO_TESTFLATSTATEMACHINE_HPP
