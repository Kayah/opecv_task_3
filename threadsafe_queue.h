#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class threadsafe_queue
{
private:    

    mutable std::mutex mt;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue() {}
    
    void push(T new_val)
    {
        std::lock_guard<std::mutex> lock(mt);
        data_queue.push(std::move(new_val));
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mt);
        data_cond.wait(lock, [this](){return !data_queue.empty();});
        value = std::move(data_queue.front()); 
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lock(mt);
        data_cond.wait(lock, [this](){return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mt);
        if (data_queue.empty())
            return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(mt);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res; 
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mt);
        return data_queue.empty();
    }
};
#endif // ifndef THREADSAFE_QUEUE_H
