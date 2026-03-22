@echo off
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
echo Building project...

if exist build (
    findstr /C:"CMAKE_GENERATOR:INTERNAL=Ninja" build\CMakeCache.txt >nul 2>&1
    if errorlevel 1 (
        rmdir /s /q build
    )
)

if not exist build mkdir build
cd build

cmake -G Ninja ..
cmake --build . --config Release
set BUILD_ERR=%ERRORLEVEL%

cd ..

if %BUILD_ERR% equ 0 (
    echo Build successful!
) else (
    echo Build failed.
    exit /b %BUILD_ERR%
)