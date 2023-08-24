#pragma once

#include <queue>
#include <mutex>

template <typename T>
class ThreadQueue
{
private:
    std::queue<T> m_que;
    std::mutex m_mtx;
public:
    // ThreadQueue(T&);
    ThreadQueue() {

    }
    ThreadQueue(ThreadQueue &q) {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_que = q;
    }
    ~ThreadQueue() {}
    void enqueue(T &e) {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_que.push(e);
    }
    bool dequeue(T &e) {
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_que.empty()) {
            return false;
        }
        e = std::move(m_que.front());
        m_que.pop();
        return true;
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_que.empty();
    }
};