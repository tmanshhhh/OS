#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace lab3 {

// Время в миллисекундах
inline std::uint64_t now_ms() {
    using namespace std::chrono;
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

// Shared state между процессами
struct SharedState {
    std::int64_t counter = 0;      // общий счётчик
    std::int64_t owner_pid = 0;    // PID главного процесса
    std::int64_t owner_heartbeat_ms = 0; // время последнего обновления heartbeat
    std::int64_t child1_pid = 0;   // PID дочернего процесса 1
    std::int64_t child2_pid = 0;   // PID дочернего процесса 2
};

// Shared memory API
SharedState* open_shared_state();
void close_shared_state();

// Глобальный lock для критической секции
class LockGuard {
public:
    LockGuard();
    ~LockGuard();

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};

// Пути
std::string shared_file_path();
std::string log_file_path();

}  // namespace lab3
