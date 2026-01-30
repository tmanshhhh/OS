#!/bin/bash

# Скрипт сборки проекта Lab3 на Linux

# Путь к корню проекта
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

echo "=== Lab3 Build (Linux) ==="
echo "Root: $ROOT_DIR"
echo "Build dir: $BUILD_DIR"

# Создать каталог сборки, если не существует
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

# Генерация makefiles через CMake
cmake -G "Unix Makefiles" "$ROOT_DIR"

# Сборка проекта
cmake --build . --config Release

echo "=== Build completed ==="
