@echo off
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
echo Building tests...

if exist build (
    findstr /C:"CMAKE_GENERATOR:INTERNAL=Ninja" build\CMakeCache.txt >nul 2>&1
    if errorlevel 1 (
        rmdir /s /q build
    )
)

if not exist build mkdir build
cd build

cmake -G Ninja ..
cmake --build . --target film_library_tests --config Debug

if %ERRORLEVEL% neq 0 (
    echo Test build failed.
    exit /b 1
)

echo Running tests...

set "TEST_PATH_MSVC_DBG=tests\Debug\film_library_tests.exe"
set "TEST_PATH_MSVC_REL=tests\Release\film_library_tests.exe"
set "TEST_PATH_MINGW=tests\film_library_tests.exe"

if exist "%TEST_PATH_MSVC_DBG%" (
    "%TEST_PATH_MSVC_DBG%"
) else if exist "%TEST_PATH_MSVC_REL%" (
    "%TEST_PATH_MSVC_REL%"
) else if exist "%TEST_PATH_MINGW%" (
    "%TEST_PATH_MINGW%"
) else (
    echo Test executable not found!
    exit /b 1
)