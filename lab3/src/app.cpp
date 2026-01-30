#include "app.h"
#include "platform.h"
#include "shared.h"

#include <iostream>
#include <sstream>
#include <thread>

namespace lab3 {

// Дочерний процесс
namespace {
void run_child(int mode, SharedState* state) {
    pid_t self = get_pid();
    log_line(time_now() + " child start mode=" + std::to_string(mode) +
             " pid=" + std::to_string(self));

    {
        LockGuard lock;
        if (mode == 1) state->counter += 10;
        else if (mode == 2) state->counter *= 2;
    }

    if (mode == 2) {
        sleep_ms(2000);
        LockGuard lock;
        state->counter /= 2;
    }

    log_line(time_now() + " child exit pid=" + std::to_string(self));
}
}

// Основная функция
void run_app(bool is_child, int child_mode) {
    SharedState* state = open_shared_state();
    if (!state) {
        std::cerr << "Cannot open shared memory\n";
        return;
    }

    pid_t self = get_pid();
    log_line(time_now() + " start pid=" + std::to_string(self));

    if (is_child) {
        run_child(child_mode, state);
        close_shared_state();
        return;
    }

    bool running = true;

    // CLI поток
    std::thread input_thread([&]() {
        std::string line;
        while (running && std::getline(std::cin, line)) {
            std::istringstream iss(line);
            std::string cmd; iss >> cmd;

            if (cmd == "set") {
                long long val; 
                if (iss >> val) {
                    LockGuard lock; state->counter = val;
                    log_line(time_now() + " pid=" + std::to_string(self) +
                             " set counter=" + std::to_string(val));
                } else std::cout << "Usage: set <number>\n";
            } else if (cmd == "exit" || cmd == "quit") {
                running = false;
            }
        }
    });

    // Поток увеличения счётчика
    std::thread counter_thread([&]() {
        while (running) {
            sleep_ms(300);
            LockGuard lock;
            state->counter += 1;
        }
    });

    uint64_t last_log = now_ms();
    uint64_t last_spawn = now_ms();

    while (running) {
        sleep_ms(100);
        uint64_t now = now_ms();

        // Owner heartbeat
        {
            LockGuard lock;
            if (state->owner_pid == 0 || !is_process_alive(state->owner_pid) ||
                now - state->owner_heartbeat_ms > 4000) {
                state->owner_pid = self;
            }
            if (state->owner_pid == self)
                state->owner_heartbeat_ms = now;
        }

        bool is_owner = false;
        {
            LockGuard lock;
            is_owner = (state->owner_pid == self);
        }

        if (!is_owner) continue;

        // Лог каждые 1 сек
        if (now - last_log >= 1000) {
            last_log = now;
            long long value;
            {
                LockGuard lock; value = state->counter;
            }
            log_line(time_now() + " pid=" + std::to_string(self) +
                     " counter=" + std::to_string(value));
        }

        // Spawn children каждые 3 сек
        if (now - last_spawn >= 3000) {
            last_spawn = now;

            bool busy = false;
            {
                LockGuard lock;
                if ((state->child1_pid && is_process_alive(state->child1_pid)) ||
                    (state->child2_pid && is_process_alive(state->child2_pid)))
                    busy = true;
                else { state->child1_pid = 0; state->child2_pid = 0; }
            }

            if (busy) {
                log_line(time_now() + " pid=" + std::to_string(self) +
                         " skip spawn: child still running");
            } else {
                pid_t c1 = spawn_child(1);
                pid_t c2 = spawn_child(2);
                {
                    LockGuard lock;
                    state->child1_pid = c1;
                    state->child2_pid = c2;
                }
                log_line(time_now() + " pid=" + std::to_string(self) +
                         " spawned children " + std::to_string(c1) + ", " +
                         std::to_string(c2));
            }
        }
    }

    if (input_thread.joinable()) input_thread.join();
    if (counter_thread.joinable()) counter_thread.join();

    log_line(time_now() + " exit pid=" + std::to_string(self));
    close_shared_state();
}

}  // namespace lab3
