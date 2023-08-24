#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "ThreadQueue.h"



class ThreadPool {
    private:
        class Worker {
            ThreadPool *m_pool;
            int m_id;
            public:
                Worker(ThreadPool *pool, int id): m_pool(pool), m_id(id){}
                void operator()() {
                    std::function<void()> func;
                    bool dequeued;
                    while (!m_pool->m_shutdown) {
                        {
                            std::unique_lock<std::mutex> lock(m_pool->m_mtx);
                            while (m_pool->m_thread_queue.empty() && !m_pool->m_shutdown) {
                                m_pool->m_conditional_lock.wait(lock, [this] {return !m_pool->m_thread_queue.empty() || m_pool->m_shutdown;});
                            }
                            dequeued = m_pool->m_thread_queue.dequeue(func);
                        }
                        if (dequeued) {
                            func();
                        }
                    }
                }
        };
        std::vector<std::thread> m_threads;
        std::mutex m_mtx;
        std::condition_variable m_conditional_lock;
        bool m_shutdown;
        ThreadQueue<std::function<void()>> m_thread_queue;
    public:
        ThreadPool(const int threads): m_threads(std::vector<std::thread>(threads)), m_shutdown(false) {}
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool& operator=(const ThreadPool &) = delete;
        ThreadPool& operator=(const ThreadPool &&) = delete;
        void init() {
            for (int i = 0; i < m_threads.size(); ++ i) {
                m_threads[i] = std::thread(Worker(this, i));
            }
        }

        void shutdown() {
            m_shutdown = true;
            m_conditional_lock.notify_all();
            for (int i = 0; i < m_threads.size(); ++ i) {
                if (m_threads[i].joinable()) {
                    m_threads[i].join();
                }
            }
        }

        template <typename F, typename...Args>
        auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
            std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
            std::function<void()> func_wrapper = [task_ptr] {
                (*task_ptr)();
            };
            {
                std::unique_lock<std::mutex> lock(m_mtx);
                m_thread_queue.enqueue(func_wrapper);
            }
            m_conditional_lock.notify_one();
            return task_ptr->get_future();
        }

};