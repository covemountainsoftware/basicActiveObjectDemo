//
// Created by Matthew Eshleman on 4/9/21.
//
#include <mutex>
#include <vector>
#include <deque>
#include "fauxQueue.h"

namespace cms
{

class StdQueue
{
public:
    using LockGuard = std::unique_lock<std::mutex>;

    explicit StdQueue(size_t queueDepth, size_t eventSize) :
        mQueueDepth(queueDepth),
        mEventSize(eventSize),
        mCondVar(),
        mMutex(),
        mQueue()
    {
    }

    ~StdQueue()
    {
    }

    size_t Count() const
    {
        LockGuard lockQueue(mMutex);
        return mQueue.size();
    }

    bool Post(const void * item)
    {
        LockGuard lockQueue(mMutex);
        auto queueSize = mQueue.size();
        if (queueSize < mQueueDepth)
        {
            EventT qEvent;
            qEvent.resize(mEventSize);
            memcpy(qEvent.data(), item, mEventSize);

            mQueue.push_back(qEvent);
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

    bool PostUrgent(const void * item)
    {
        LockGuard lockQueue(mMutex);
        auto queueSize = mQueue.size();
        if (queueSize < mQueueDepth)
        {
            EventT qEvent;
            qEvent.resize(mEventSize);
            memcpy(qEvent.data(), item, mEventSize);

            mQueue.push_front(qEvent);
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

    bool Receive(void *pvBuffer)
    {
        LockGuard lockQueue(mMutex);

        EventT event;

        while (mQueue.empty())
        {
            mCondVar.wait(lockQueue);
        }

        event = mQueue.front();
        mQueue.pop_front();

        memcpy(pvBuffer, event.data(), mEventSize);

        lockQueue.unlock();

        return true;
    }

private:
    using EventT = std::vector<uint8_t>;
    const size_t mQueueDepth;
    const size_t mEventSize;
    std::condition_variable mCondVar;
    mutable std::mutex mMutex;
    std::deque<EventT> mQueue;
};

} // namespace cms


QueueHandle_t xQueueCreate(size_t uxQueueLength, size_t uxItemSize)
{
    auto queue = new cms::StdQueue(uxQueueLength, uxItemSize);
    return queue;
}

void vQueueDelete( QueueHandle_t xQueue )
{
    auto queue = static_cast<cms::StdQueue*>(xQueue);
    if (queue != nullptr)
    {
        delete queue;
    }
}

bool xQueueSendToBack(QueueHandle_t xQueue, const void* pvItemToQueue)
{
    auto queue = static_cast<cms::StdQueue*>(xQueue);
    if (queue == nullptr)
    {
        return false;
    }

    return queue->Post(pvItemToQueue);
}

bool xQueueSendToFront(QueueHandle_t xQueue, const void* pvItemToQueue)
{
    auto queue = static_cast<cms::StdQueue*>(xQueue);
    if (queue == nullptr)
    {
        return false;
    }

    return queue->PostUrgent(pvItemToQueue);
}

bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer)
{
    auto queue = static_cast<cms::StdQueue*>(xQueue);
    if (queue == nullptr)
    {
        return false;
    }

    return queue->Receive(pvBuffer);
}

size_t uxQueueMessagesWaiting(const QueueHandle_t xQueue)
{
    auto queue = static_cast<cms::StdQueue*>(xQueue);
    if (queue == nullptr)
    {
        return 0;
    }

    return queue->Count();
}
