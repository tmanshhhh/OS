@echo off
setlocal enabledelayedexpansion

REM Папка скрипта
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

echo ===============================
echo Pulling latest changes from Git
echo ===============================
git pull

REM Проверяем CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo CMake not found! Please install CMake and add it to PATH
    exit /b 1
)

REM Создаём папку сборки
if not exist build mkdir build
cd build

echo ===============================
echo Configuring project with CMake
echo ===============================
cmake -G "Ninja" ..

echo ===============================
echo Building project
echo ===============================
cmake --build .

if %ERRORLEVEL% equ 0 (
    echo ===============================
    echo Build successful!
    echo ===============================

    echo.
    set /p RUN_TESTS="Do you want to run the test utility? [y/N] "
    if /i "!RUN_TESTS!"=="y" (
        echo ===============================
        echo Running test utility
        echo ===============================

        set EXE_PATH=%CD%\test_runner.exe
        if not exist %EXE_PATH% set EXE_PATH=%CD%\bin\test_runner.exe

        if exist %EXE_PATH% (
            %EXE_PATH%
        ) else (
            echo Cannot find test_runner.exe!
        )

    )
) else (
    echo ===============================
    echo Build failed!
    echo ===============================
    exit /b 1
)

endlocal
pause
