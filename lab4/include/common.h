#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace lab4 {

// === Время ===
using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

// Текущее время в миллисекундах с эпохи
std::int64_t now_ms();

// Форматирование времени для логов (ISO-подобный формат)
std::string iso_time(const TimePoint& tp);

// === Пути ===
std::string data_dir();
std::string measurements_log_path();
std::string hourly_log_path();
std::string daily_log_path();

// === Измерение температуры ===
struct Measurement {
    TimePoint timestamp;
    double value = 0.0;
};

// === Аккумулятор для среднего значения ===
class Accumulator {
public:
    void add(double value);
    double average() const;
    std::size_t count() const;
    void reset();

private:
    double sum_ = 0.0;
    std::size_t count_ = 0;
};

}  // namespace lab4
