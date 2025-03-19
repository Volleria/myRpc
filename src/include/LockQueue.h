#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

template <typename T>
class LockQueue
{
public:
    // 将数据放入队列
    void Push(const T &data) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(data);
        _condition.notify_one();  // 通知一个等待的线程
    }

    // 支持移动语义的 Push
    void Push(T &&data) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(data));
        _condition.notify_one();  // 通知一个等待的线程
    }

    // 从队列中取出数据（阻塞直到有数据）
    T Pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [this]() { return !_queue.empty(); });

        T data = std::move(_queue.front());  // 使用移动语义避免拷贝
        _queue.pop();
        return data;
    }

    // 从队列中取出数据（带超时）
    bool Pop(T &data, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_condition.wait_for(lock, timeout, [this]() { return !_queue.empty(); })) {
            return false;  // 超时
        }

        data = std::move(_queue.front());  // 使用移动语义避免拷贝
        _queue.pop();
        return true;
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condition;
};
