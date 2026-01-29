#!/bin/bash

# Папка скрипта
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "=============================="
echo "Pulling latest changes from Git"
echo "=============================="
git pull

# Создаём папку сборки
mkdir -p build
cd build

echo "=============================="
echo "Configuring project with CMake"
echo "=============================="
cmake ..

echo "=============================="
echo "Building project"
echo "=============================="
cmake --build .

if [ $? -eq 0 ]; then
    echo "=============================="
    echo "Build successful!"
    echo "=============================="

    echo
    read -p "Do you want to run the test utility? [y/N] " response
    if [[ "$response" =~ ^([yY][eE][sS]|[yY])+$ ]]; then
        echo "=============================="
        echo "Running test utility"
        echo "=============================="
        ./test_runner
    fi
else
    echo "=============================="
    echo "Build failed!"
    echo "=============================="
    exit 1
fi
