@echo off
setlocal ENABLEEXTENSIONS

REM Usage: build.cmd [BuildType]
REM Default: Debug
REM Requires: git, cmake, MinGW (or Ninja)

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Debug

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%

echo Updating repository...
pushd %PROJECT_ROOT%
git pull --rebase
if errorlevel 1 goto :error
popd

echo Configuring CMake (%BUILD_TYPE%)...
cmake -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%\build" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 goto :error

echo Building project...
cmake --build "%PROJECT_ROOT%\build" --config %BUILD_TYPE%
if errorlevel 1 goto :error

echo Done. Binaries are in %PROJECT_ROOT%\build\bin
exit /b 0

:error
echo Build failed. See output above.
exit /b 1
