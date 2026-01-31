#include "logging.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "time_utils.h"

namespace lab4 {


std::string format_line(const Measurement& m) {
    auto ms_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            m.timestamp.time_since_epoch()
        ).count();

    std::ostringstream oss;
    oss << ms_epoch << ';'
        << iso_time(m.timestamp) << ';'
        << std::fixed << std::setprecision(2) << m.value;

    return oss.str();
}

std::optional<std::int64_t> parse_epoch_ms(const std::string& line) {
    std::istringstream iss(line);
    std::string token;

    if (!std::getline(iss, token, ';')) return std::nullopt;

    try {
        return std::stoll(token);
    } catch (...) {
        return std::nullopt;
    }
}


static void append_line(const std::string& path, const std::string& line) {
    std::ofstream out(path, std::ios::app);
    if (out.is_open()) out << line << '\n';
}

static void prune_log(const std::string& path, std::int64_t cutoff_ms) {
    std::ifstream in(path);
    if (!in.is_open()) return;

    std::vector<std::string> keep;
    std::string line;

    while (std::getline(in, line)) {
        auto ms = parse_epoch_ms(line);
        if (ms && *ms >= cutoff_ms) keep.push_back(line);
    }
    in.close();

    std::ofstream out(path, std::ios::trunc);
    for (const auto& l : keep) out << l << '\n';
}


void write_measurement(const Measurement& m) {
    append_line(measurements_log_path(), format_line(m));
}

void write_hourly_average(double value, const TimePoint& tp) {
    Measurement m{tp, value};
    append_line(hourly_log_path(), format_line(m));
}

void write_daily_average(double value, const TimePoint& tp) {
    Measurement m{tp, value};
    append_line(daily_log_path(), format_line(m));
}


void prune_measurements_log(std::chrono::hours keep) {
    auto cutoff = now_ms() - std::chrono::duration_cast<std::chrono::milliseconds>(keep).count();
    prune_log(measurements_log_path(), cutoff);
}

void prune_hourly_log(std::chrono::hours keep) {
    auto cutoff = now_ms() - std::chrono::duration_cast<std::chrono::milliseconds>(keep).count();
    prune_log(hourly_log_path(), cutoff);
}

void prune_daily_log(int year) {
    std::ifstream in(daily_log_path());
    if (!in.is_open()) return;

    std::vector<std::string> keep;
    std::string line;

    while (std::getline(in, line)) {
        auto ms = parse_epoch_ms(line);
        if (!ms) continue;

        TimePoint tp{std::chrono::milliseconds(*ms)};
        if (year_of(tp) == year) keep.push_back(line);
    }
    in.close();

    std::ofstream out(daily_log_path(), std::ios::trunc);
    for (const auto& l : keep) out << l << '\n';
}

}  // namespace lab4
