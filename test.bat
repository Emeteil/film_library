@echo off
echo Building tests...

if not exist build mkdir build
cd build
cmake ..
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