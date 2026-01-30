#include "platform.h"
#include "shared.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

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
#include <sys/wait.h>
#include <signal.h>
#endif

namespace fs = std::filesystem;

namespace lab3 {

// Helpers
namespace {
void ensure_dir(const fs::path& p) {
    std::error_code ec;
    fs::create_directories(p, ec);
}
} // namespace

// Process & sleep
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

// Time string 
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

// Paths
std::string data_dir() {
    static fs::path dir = fs::current_path() / "lab3";
    ensure_dir(dir);
    return dir.string();
}

std::string shared_file_path() {
    return (fs::path(data_dir()) / "shared.bin").string();
}

std::string log_file_path() {
    fs::path p = fs::path(data_dir()) / "logs";
    ensure_dir(p);
    return (p / "app.log").string();
}

// Logging 
void log_line(const std::string& line) {
    std::ofstream out(log_file_path(), std::ios::app);
    out << line << std::endl;
}

// Shared memory 
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

    ftruncate(fd, sizeof(SharedState));
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

// Global lock 
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

// Spawn child 
pid_t spawn_child(int mode) {
#ifdef _WIN32
    fs::path exe_path = fs::current_path() / "lab3.exe";
    std::string exe = exe_path.string();

    std::ostringstream cmd;
    cmd << "\"" << exe << "\" --child " << mode;

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    std::string cmdline = cmd.str();
    if (!CreateProcessA(
            nullptr, cmdline.data(),
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
        execlp("./lab3", "./lab3", "--child",
               (mode == 1 ? "1" : "2"), nullptr);
        _exit(1);
    }
    return pid;
#endif
}

// Check if process alive 
bool is_process_alive(pid_t pid) {
#ifdef _WIN32
    if (pid <= 0) return false;
    HANDLE h = OpenProcess(SYNCHRONIZE, FALSE, static_cast<DWORD>(pid));
    if (!h) return false;
    DWORD code = WaitForSingleObject(h, 0);
    CloseHandle(h);
    return code == WAIT_TIMEOUT;
#else
    if (pid <= 0) return false;
    int r = kill(static_cast<pid_t>(pid), 0);
    return r == 0 || errno == EPERM;
#endif
}

} // namespace lab3
