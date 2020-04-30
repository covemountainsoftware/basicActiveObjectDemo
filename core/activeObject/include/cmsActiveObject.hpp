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

#ifndef CMSACTIVEOBJECT_HPP
#define CMSACTIVEOBJECT_HPP

#include <cstdint>
#include <atomic>
#include "cmsFlatStateMachine.hpp"

namespace cms
{

enum class ThreadPriority
{
    IDLE,
    LOWEST,
    LOW,
    NORMAL,
    HIGH,
    HIGHEST,
    TIME_CRITICAL
};

enum class ProcessOption
{
    NORMAL,
    UNIT_TEST
};

template<typename EventT, uint32_t QueueDepth>
class ActiveObject : public cms::FlatStateMachine<EventT>
{
public:
    ActiveObject() = default;
    virtual ~ActiveObject() = default;
    ActiveObject(const ActiveObject&) = delete;
    ActiveObject& operator=(const ActiveObject&) = delete;
    ActiveObject(ActiveObject&&) = delete;
    ActiveObject& operator=(ActiveObject&&) = delete;

    /**
     * @brief Start the active object
     * @param option NORMAL: create a thread, initialize the statemachine from the thread context, and start processing events
     *               UNIT_TEST: does not create a thread, initializes the statemachine immediately. Processing events must
     *                          be manually initiated from the unit tests.
     * @return true - success.
     */
    virtual bool Start(ProcessOption option = ProcessOption::NORMAL) = 0;

    /**
     * @brief Post an event, FIFO, to the active object's event queue
     * @param event - the event to post
     * @return true - the event was successfully posted.
     */
    virtual bool Post(const EventT& event) = 0;

    /**
     * @brief Post an event, LIFO, to the active object's event queue
     * @param event - the event to post
     * @return true - the event was successfully posted.
     */
    virtual bool PostUrgent(const EventT& event) = 0;

    /**
     * @brief Set the priority of the active object
     * @param priority
     * @return
     */
    virtual bool SetPriority(ThreadPriority priority) = 0;

    /**
    * @brief Stop() - stop the active object and any associated thread context.
    *                 MUST be called from the most derived class before
    *                 the object is destroyed.
    *
    * @note - most embedded systems never destroy an active object
    */
    virtual void Stop() = 0;

    /**
     * @brief intended for unit test access only.
     */
    virtual bool ProcessOneEvent(ProcessOption option = ProcessOption::NORMAL) = 0;

protected:
    void ExitTask()
    {
        mExit = true;
    }

    void Task()
    {
        cms::FlatStateMachine<EventT>::Initialize();
        while (!mExit)
        {
            ProcessOneEvent(ProcessOption::NORMAL);
        }
    }

private:
    std::atomic_bool mExit{ false };
};

} //namespace

#endif //CMSACTIVEOBJECT_HPP
