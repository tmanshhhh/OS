#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "common.h"

namespace lab4 {

class Simulator {
public:
    Simulator() = default;
    ~Simulator();

    // Запуск и остановка симулятора
    void start();
    void stop();

    // Получение следующего измерения
    // Возвращает false, если симулятор остановлен
    bool pop(Measurement& out);

private:
    void run();

    std::atomic<bool> running_{false};
    std::thread worker_;

    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<Measurement> queue_;
};

}  // namespace lab4
