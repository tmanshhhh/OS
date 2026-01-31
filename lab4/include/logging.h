#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

#include "common.h"

namespace lab4 {

std::string format_line(const Measurement& m);

std::optional<std::int64_t> parse_epoch_ms(const std::string& line);

// Запись нового измерения в лог
void write_measurement(const Measurement& m);

// Запись среднего значения за час
void write_hourly_average(double value, const TimePoint& tp);

// Запись среднего значения за день
void write_daily_average(double value, const TimePoint& tp);

// Очистка логов по времени хранения
void prune_measurements_log(std::chrono::hours keep);  // 24 часа
void prune_hourly_log(std::chrono::hours keep);        // месяц ≈ 720 часов
void prune_daily_log(int year);                        // текущий год

}  // namespace lab4
