#include "platform.h"
#include "shared.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#endif

namespace fs = std::filesystem;

namespace lab3 {

// helpers
namespace {

fs::path executable_dir() {
#ifdef _WIN32
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return fs::path(buf).parent_path();
#else
    char buf[4096]{};
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len <= 0) return fs::current_path();
    buf[len] = '\0';
    return fs::path(buf).parent_path();
#endif
}

void ensure_dir(const fs::path& p) {
    std::error_code ec;

    if (fs::exists(p, ec)) {
        if (!fs::is_directory(p, ec)) {
            std::cerr << "Path exists but is not a directory: " << p << std::endl;
        }
        return;
    }

    fs::create_directories(p, ec);
    if (ec) {
        std::cerr << "Failed to create directory: " << p
                  << " (" << ec.message() << ")" << std::endl;
    }
}

} // namespace

// process
pid_t get_pid() {
#ifdef _WIN32
    return static_cast<pid_t>(GetCurrentProcessId());
#else
    return static_cast<pid_t>(getpid());
#endif
}

void sleep_ms(std::uint32_t ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// time
std::string time_now() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

// paths
fs::path data_dir_path() {
    static fs::path dir = executable_dir() / "data";
    ensure_dir(dir);
    return dir;
}

std::string data_dir() {
    return data_dir_path().string();
}

std::string shared_file_path() {
    return (data_dir_path() / "shared.bin").string();
}

std::string log_file_path() {
    fs::path logs = data_dir_path() / "logs";
    ensure_dir(logs);

    fs::path log = logs / "app.log";
    std::ofstream out(log, std::ios::app);
    if (!out) {
        std::cerr << "Cannot create log file: " << log << std::endl;
    }
    return log.string();
}

// logging 
void log_line(const std::string& line) {
    fs::path path = log_file_path();
    std::ofstream out(path, std::ios::app);
    if (!out) {
        std::cerr << "Cannot open log file: " << path << std::endl;
        return;
    }
    out << line << std::endl;
}

// shared memory
static SharedState* g_shared = nullptr;

SharedState* open_shared_state() {
#ifdef _WIN32
    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
        0, sizeof(SharedState), "Lab3SharedMemory");

    if (!hMap) return nullptr;

    void* ptr = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState));
    if (!ptr) return nullptr;

    g_shared = static_cast<SharedState*>(ptr);
#else
    int fd = shm_open("/lab3_shared", O_CREAT | O_RDWR, 0666);
    if (fd < 0) return nullptr;

    if (ftruncate(fd, sizeof(SharedState)) != 0) return nullptr;

    void* ptr = mmap(nullptr, sizeof(SharedState),
                     PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) return nullptr;

    g_shared = static_cast<SharedState*>(ptr);
#endif
    return g_shared;
}

void close_shared_state() {
#ifdef _WIN32
    if (g_shared) {
        UnmapViewOfFile(g_shared);
        g_shared = nullptr;
    }
#else
    if (g_shared) {
        munmap(g_shared, sizeof(SharedState));
        g_shared = nullptr;
    }
#endif
}

// lock
#ifdef _WIN32
static HANDLE g_mutex = nullptr;
#else
#include <pthread.h>
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

LockGuard::LockGuard() {
#ifdef _WIN32
    if (!g_mutex) {
        g_mutex = CreateMutexA(nullptr, FALSE, "Lab3GlobalMutex");
    }
    WaitForSingleObject(g_mutex, INFINITE);
#else
    pthread_mutex_lock(&g_mutex);
#endif
}

LockGuard::~LockGuard() {
#ifdef _WIN32
    ReleaseMutex(g_mutex);
#else
    pthread_mutex_unlock(&g_mutex);
#endif
}

// spawn
pid_t spawn_child(int mode) {
#ifdef _WIN32
    fs::path exe = executable_dir() / "lab3.exe";

    std::ostringstream cmd;
    cmd << "\"" << exe.string() << "\" --child " << mode;

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    std::string cmdline = cmd.str();
    if (!CreateProcessA(nullptr, cmdline.data(),
                        nullptr, nullptr, FALSE, 0,
                        nullptr, nullptr, &si, &pi)) {
        return 0;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return static_cast<pid_t>(pi.dwProcessId);
#else
    pid_t pid = fork();
    if (pid == 0) {
        execl("./lab3", "./lab3", "--child",
              mode == 1 ? "1" : "2", nullptr);
        _exit(1);
    }
    return pid;
#endif
}

// check 
bool is_process_alive(pid_t pid) {
    if (pid <= 0) return false;
#ifdef _WIN32
    HANDLE h = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!h) return false;
    DWORD r = WaitForSingleObject(h, 0);
    CloseHandle(h);
    return r == WAIT_TIMEOUT;
#else
    int r = kill(pid, 0);
    return r == 0 || errno == EPERM;
#endif
}

} // namespace lab3
