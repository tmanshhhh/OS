@echo off
REM Скрипт сборки проекта Lab3 на Windows 

SET ROOT_DIR=%~dp0\..
SET BUILD_DIR=%ROOT_DIR%\build

echo === Lab3 Build (Windows) ===
echo Root: %ROOT_DIR%
echo Build dir: %BUILD_DIR%

REM Создать каталог сборки
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Генерация проекта
cmake -G "Ninja" "%ROOT_DIR%"

REM Сборка
cmake --build . --config Release

echo === Build completed ===
pause
