@echo off
call build.bat
if %ERRORLEVEL% equ 0 (
    call run.bat
)