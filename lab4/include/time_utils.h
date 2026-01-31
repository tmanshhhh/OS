#pragma once

#include "common.h"

namespace lab4 {

// Возвращает час (0–23) по временной метке
int hour_of(const TimePoint& tp);

// Возвращает номер дня в году (0–365)
int day_of_year(const TimePoint& tp);

// Возвращает календарный год 
int year_of(const TimePoint& tp);

}  // namespace lab4
