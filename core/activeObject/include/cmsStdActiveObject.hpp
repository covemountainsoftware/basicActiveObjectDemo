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

#ifndef CMSSTDACTIVEOBJECT_HPP
#define CMSSTDACTIVEOBJECT_HPP

#include "cmsActiveObject.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

namespace cms
{

/**
 * @brief The StdActiveObject provides for a std library
 *        based active object.
 * @tparam EventT
 * @tparam QueueDepth
 */
template<typename EventT, uint32_t QueueDepth>
class StdActiveObject : public ActiveObject<EventT, QueueDepth>
{
public:
    using LockGuard = std::unique_lock<std::mutex>;

    bool Start(ProcessOption option = ProcessOption::WAIT) override
    {
        if (option == ProcessOption::WAIT)
        {
            if (mThread == nullptr)
            {
                mThread = new std::thread([=]() { this->Task(); });
            }
        }
        else
        {
            this->Initialize();
        }
        return true;
    }

    bool Post(const EventT& event) override
    {
        LockGuard lockQueue(mMutex);
        auto queueSize = mQueue.size();
        if (queueSize < QueueDepth)
        {
            mQueue.push_back(event);
            lockQueue.unlock();

            if (queueSize == 0)
            {
                mCondVar.notify_one();
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool PostUrgent(const EventT& event) override
    {
        LockGuard lockQueue(mMutex);
        auto queueSize = mQueue.size();
        if (queueSize < QueueDepth)
        {
            mQueue.push_front(event);
            lockQueue.unlock();

            if (queueSize == 0)
            {
                mCondVar.notify_one();
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool SetPriority(ThreadPriority priority) override
    {
        (void)priority;
        //TODO
        return false;
    }

    bool ProcessOneEvent(ProcessOption option) override
    {
        LockGuard lockQueue(mMutex);

        EventT event;
        if (option == ProcessOption::WAIT)
        {
            while (mQueue.empty())
            {
                mCondVar.wait(lockQueue);
            }
        }
        else
        {
            if (mQueue.size() == 0)
            {
                return false;
            }
        }

        event = mQueue.front();
        mQueue.pop_front();

        lockQueue.unlock();
        FlatStateMachine<EventT>::ProcessEvent(&event);
        return true;
    }

private:
    std::thread* mThread = nullptr;
    std::condition_variable mCondVar;
    std::mutex mMutex;
    std::deque<EventT> mQueue;
};

} // namespace cms

#endif //CMSSTDACTIVEOBJECT_HPP
