#include "threading/thread-pool.h"

namespace mandelbrot
{

    ThreadPool::ThreadPool(int numThreads) :
        m_mutex(),
        m_cv(),
        m_threads(),
        m_tasks(),
        m_working(true)
    {
        m_threads.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
            m_threads.emplace_back(std::bind(&ThreadPool::threadJob, this));
    }

    ThreadPool::~ThreadPool()
    {
        m_working = false;
        m_cv.notify_all();

        for (auto& thread : m_threads)
            thread.join();
    }

    void ThreadPool::post(std::function<void()> &&work)
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_tasks.push_back(std::move(work));
        m_cv.notify_one();
    }

    void ThreadPool::threadJob()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_cv.wait(lock, [this](){
                return !m_tasks.empty() || !m_working;
            });

            if (!m_working && m_tasks.empty())
                break;

            std::function<void()> task = std::move(m_tasks.front());
            m_tasks.pop_front();
            lock.unlock();

            task();

            if (!m_working && m_tasks.empty())
                break;
        }
    }

}
