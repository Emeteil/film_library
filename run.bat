@echo off

set "APP_PATH_MSVC_DBG=build\src\app\Debug\FilmLibrary.exe"
set "APP_PATH_MSVC_REL=build\src\app\Release\FilmLibrary.exe"
set "APP_PATH_MINGW=build\src\app\FilmLibrary.exe"

if exist "%APP_PATH_MSVC_REL%" (
    set "APP_PATH=%APP_PATH_MSVC_REL%"
) else if exist "%APP_PATH_MSVC_DBG%" (
    set "APP_PATH=%APP_PATH_MSVC_DBG%"
) else if exist "%APP_PATH_MINGW%" (
    set "APP_PATH=%APP_PATH_MINGW%"
) else (
    echo Executable not found! Please run build.bat first.
    exit /b 1
)

echo Starting FilmLibrary...
"%APP_PATH%"