#include "simulator.h"

#include <random>
#include <chrono>

namespace lab4 {

Simulator::~Simulator() {
    stop();
}

void Simulator::start() {
    if (running_) return;

    running_ = true;
    worker_ = std::thread(&Simulator::run, this);
}

void Simulator::stop() {
    running_ = false;
    cv_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }
}

bool Simulator::pop(Measurement& out) {
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [&]() {
        return !queue_.empty() || !running_;
    });

    if (queue_.empty()) {
        return false;
    }

    out = queue_.front();
    queue_.pop();
    return true;
}

void Simulator::run() {
    std::mt19937 rng(
        static_cast<unsigned>(std::chrono::system_clock::now()
                                  .time_since_epoch()
                                  .count())
    );
    std::uniform_real_distribution<double> dist(-10.0, 35.0);

    while (running_) {
        Measurement m;
        m.timestamp = Clock::now();
        m.value = dist(rng);

        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(m);
        }

        cv_.notify_one();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    cv_.notify_all();
}

}  // namespace lab4
