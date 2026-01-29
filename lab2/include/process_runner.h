#pragma once

#include <string>
#include <vector>

namespace process_runner {

/**
 * Дескриптор запущенного процесса
 */
struct ProcessHandle {
#ifdef _WIN32
    void* process = nullptr;  // HANDLE (Windows)
#else
    int pid = -1;             // PID (POSIX)
#endif
};

/**
 * Запуск программы в фоновом режиме.
 * args[0] — путь к исполняемому файлу,
 * args[1..] — аргументы командной строки.
 *
 * @param args   команда и аргументы
 * @param handle дескриптор процесса
 * @param err    строка для сообщения об ошибке (опционально)
 * @return true при успешном запуске
 */
bool start(
    const std::vector<std::string>& args,
    ProcessHandle& handle,
    std::string* err = nullptr
);

/**
 * Ожидание завершения процесса.
 *
 * @param handle     дескриптор процесса
 * @param exit_code код завершения
 * @param timeout_ms таймаут в миллисекундах,
 *                   timeout_ms < 0 — ждать бесконечно
 * @param err        строка для сообщения об ошибке (опционально)
 * @return true при успешном ожидании
 */
bool wait(
    ProcessHandle& handle,
    int& exit_code,
    int timeout_ms = -1,
    std::string* err = nullptr
);

/**
 * Освобождение ресурсов дескриптора процесса
 */
void close(ProcessHandle& handle);

} // namespace process_runner
