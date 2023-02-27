#pragma once

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() {}
    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        std::unique_lock<std::mutex> lock(other.mutex);
        this->vecter = other.vecter;
    }

    void Push(T value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        this->vecter.emplace_back(value);
    }

    void Erase(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        this->vecter.erase
        (
            std::remove(this->vecter.begin(), this->vecter.end(), value),
            this->vecter.end()
        );
    }

    bool Empty() const
    {
        std::unique_lock<std::mutex> lock(mutex);
        return this->vecter.empty();
    }

    int Size() const
    {
        std::unique_lock<std::mutex> lock(mutex);
        return this->vecter.size();
    }

    bool TryPop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (this->vecter.empty())
        {
            return false;
        }

        value = this->vecter.front();
        this->Erase(this->vecter.front());
        return true;
    }

private:
    std::vector<T> vecter;
    mutable std::mutex mutex;
};