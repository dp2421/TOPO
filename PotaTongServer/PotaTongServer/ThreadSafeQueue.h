#pragma once

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() {}
    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        std::unique_lock<std::mutex> lock(other.mutex);
        queue = other.queue;
    }

    void Push(T value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(value);
    }

    bool Empty() const
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    int Size() const
    {
        return queue.size();
    }

    bool TryPop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty())
        {
            return false;
        }

        value = queue.front();
        queue.pop();
        return true;
    }

private:
    std::queue<T> queue;
    mutable std::mutex mutex;
};