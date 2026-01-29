#pragma once

#include <string>
#include <vector>

namespace process_runner {

// Дескриптор процесса
struct ProcessHandle {
#ifdef _WIN32
    void* process = nullptr;  // HANDLE на Windows
#else
    int pid = -1;             // PID на POSIX
#endif
};

// Запускает процесс в фоновом режиме.
// args[0] — исполняемый файл, остальные — аргументы.
// handle — дескриптор для последующего управления процессом.
// err — необязательный вывод ошибок.
// Возвращает true, если процесс успешно запущен.
bool start(const std::vector<std::string>& args, ProcessHandle& handle, std::string* err = nullptr);

// Ждёт завершения процесса.
// exit_code — код завершения процесса.
// timeout_ms < 0 — ждать бесконечно.
// Возвращает true, если процесс завершился успешно.
bool wait(ProcessHandle& handle, int& exit_code, int timeout_ms = -1, std::string* err = nullptr);

// Освобождает ресурсы, связанные с дескриптором процесса.
void close(ProcessHandle& handle);

} // namespace process_runner
