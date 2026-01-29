#!/bin/bash

cd "$(dirname "$0")"

echo "=============================="
echo "Pulling latest changes from Git"
echo "=============================="
git pull

mkdir -p build
cd build

echo "=============================="
echo "Configuring project with CMake"
echo "=============================="
cmake ..

echo "=============================="
echo "Building project"
echo "=============================="
make

echo "=============================="
echo "Running the program"
echo "=============================="
./hello_world_cpp

echo "=============================="
echo "Done!"
echo "=============================="
