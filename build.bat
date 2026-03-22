@echo off
echo Building project...

if not exist build mkdir build
cd build

cmake ..
cmake --build . --config Release

if %ERRORLEVEL% equ 0 (
    echo Build successful!
) else (
    echo Build failed.
    exit /b 1
)