#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <iostream>
#include <thread>
#include <atomic>
#include "threadsafe_queue.h"
#include <vector>
#include <stdint.h>

class join_threads
{
private:
    std::vector<std::thread>& threads;

public:
    explicit join_threads(std::vector<std::thread>& threads_):
        threads(threads_) {}

    ~join_threads()
    {
        for (size_t i = 0; i < threads.size(); i++)
        {
            if (threads[i].joinable())
            {
                threads[i].join();
            }
        }
    }
};

class thread_pool
{
private:
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {       
        while(!done)
        {
            std::function<void()> task;
            if (work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool():done(false), joiner(threads)
    {
        uint32_t hw_thr = std::thread::hardware_concurrency();
        try
        {
            for (uint32_t i = 0; i < hw_thr; i++)
            {
                threads.push_back(std::thread(&thread_pool::worker_thread, this));
            }
        }
        catch(...)
        {
            done = true;
            throw;
        }
    }

    ~thread_pool()
    {
        done = true;
    }

    bool check_job()
    {
        return done;
    }

    template<typename FunctionType>
    void submit_task(FunctionType f)
    {
        work_queue.push(std::function<void()>(f));
    }
};
#endif // ifndef THREAD_POOL_H
