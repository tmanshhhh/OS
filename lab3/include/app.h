#pragma once

namespace lab3 {

// Запуск приложения.
// is_child = true  → процесс является дочерним
// child_mode = 1   → увеличить счётчик на 10
// child_mode = 2   → умножить на 2, через 2 сек поделить на 2
void run_app(bool is_child, int child_mode);

}  // namespace lab3
