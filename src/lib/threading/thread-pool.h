#ifndef _MANDELBROT_LIB_THREADING_THREAD_POOL_H_
#define _MANDELBROT_LIB_THREADING_THREAD_POOL_H_

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace mandelbrot
{

class ThreadPool
{
public:
    /// Constructs the thread pool with a given number of worker threads
    explicit ThreadPool(int numThreads);

    /// Kills the thread pool
    ~ThreadPool();

    /// Posts a task to the end of the work queue
    void post(std::function<void()> &&work);

private:
    /// Thread execution loop. Takes jobs from the queue to be performed
    void threadJob();

private:
    /// Mutex
    std::mutex m_mutex;

    /// Condition variable
    std::condition_variable m_cv;

    /// Worker threads
    std::vector<std::thread> m_threads;

    /// Pending tasks
    std::deque<std::function<void()>> m_tasks;

    /// Worker flag - when set to false, the worker thread will halt
    bool m_working;
};

}

#endif // _MANDELBROT_LIB_THREADING_THREAD_POOL_H_
