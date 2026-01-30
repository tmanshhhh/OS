#pragma once

#include <cstdint>
#include <string>

namespace lab3 {

// Тип PID
using pid_t = std::int64_t;

// Process & time
pid_t get_pid();
void sleep_ms(std::uint32_t ms);

// Текущее время в формате YYYY-MM-DD HH:MM:SS.mmm
std::string time_now();

// Logging
void log_line(const std::string& line);

// Child process management
pid_t spawn_child(int mode);
bool is_process_alive(pid_t pid);

// Paths
std::string data_dir();
std::string shared_file_path();
std::string log_file_path();

}  // namespace lab3
