#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

#include "common.h"
#include "logging.h"
#include "time_utils.h"
#include "simulator.h"

using namespace std::chrono;

namespace lab4 {

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running = false;
}
}  // namespace

int run(bool simulate) {
    // обработка сигналов завершения
    std::signal(SIGINT, signal_handler);
#ifndef _WIN32
    std::signal(SIGTERM, signal_handler);
#endif

    std::cout << "Lab4 temperature logger started. Mode: "
              << (simulate ? "simulation" : "stdin") << std::endl;

    Simulator simulator;
    if (simulate) {
        simulator.start();
    }

    Accumulator hour_acc;
    Accumulator day_acc;

    auto now = Clock::now();
    int last_hour = hour_of(now);
    int last_day  = day_of_year(now);
    int last_year = year_of(now);

    while (running) {
        Measurement m;
        bool has_data = false;

        if (simulate) {
            has_data = simulator.pop(m);
        } else {
            double value;
            if (!(std::cin >> value)) {
                std::this_thread::sleep_for(milliseconds(100));
                continue;
            }
            m.timestamp = Clock::now();
            m.value = value;
            has_data = true;
        }

        if (!has_data) {
            std::this_thread::sleep_for(milliseconds(50));
            continue;
        }

        int h = hour_of(m.timestamp);
        int d = day_of_year(m.timestamp);
        int y = year_of(m.timestamp);

        // переход часа
        if (h != last_hour && hour_acc.count() > 0) {
            write_hourly_average(hour_acc.average(), m.timestamp);
            hour_acc.reset();
        }

        // переход дня
        if (d != last_day && day_acc.count() > 0) {
            write_daily_average(day_acc.average(), m.timestamp);
            day_acc.reset();
        }

        last_hour = h;
        last_day  = d;
        last_year = y;

        hour_acc.add(m.value);
        day_acc.add(m.value);

        write_measurement(m);

        prune_measurements_log(hours(24));
        prune_hourly_log(std::chrono::hours(24*30));  
        prune_daily_log(last_year);
    }

    if (simulate) {
        simulator.stop();
    }

    // финальный сброс
    if (hour_acc.count() > 0) {
        write_hourly_average(hour_acc.average(), Clock::now());
    }
    if (day_acc.count() > 0) {
        write_daily_average(day_acc.average(), Clock::now());
    }

    return 0;
}

}  // namespace lab4

int main(int argc, char* argv[]) {
    bool simulate = false;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--simulate") {
            simulate = true;
        }
    }

    return lab4::run(simulate);
}
