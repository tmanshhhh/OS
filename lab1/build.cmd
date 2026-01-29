@echo off

cd /d %~dp0

echo ===============================
echo Pulling latest changes from Git
echo ===============================
git pull

if not exist build (
    mkdir build
)
cd build

echo ===============================
echo Configuring project with CMake
echo ===============================
cmake -G Ninja ..

echo ===============================
echo Building project
echo ===============================
ninja

echo ===============================
echo Running the program
echo ===============================
hello_world_cpp.exe

echo ===============================
echo Done!
echo ===============================
pause
