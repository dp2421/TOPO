#pragma once

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() {}
    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        lock_guard<mutex> lock{ other.lock };
        this->vecter = other.vecter;
    }

    void Push(T value)
    {
        lock_guard<mutex> lock{ this->lock };
        this->vecter.emplace_back(value);
    }

    void Erase(T& value)
    {
        lock_guard<mutex> lock{ this->lock };
        this->vecter.erase
        (
            std::remove(this->vecter.begin(), this->vecter.end(), value),
            this->vecter.end()
        );
    }

    bool Empty() const
    {
        lock_guard<mutex> lock{ this->lock };
        return this->vecter.empty();
    }

    int Size() const
    {
        lock_guard<mutex> lock{ this->lock };
        return this->vecter.size();
    }

    bool TryPop(T& value)
    {
        {
            lock_guard<mutex> lock{ this->lock };
            if (this->vecter.empty())
            {
                return false;
            }

            value = this->vecter.front();
        }
        this->Erase(this->vecter.front());
        return true;
    }

private:
    std::vector<T> vecter;
    mutable std::mutex lock;
};