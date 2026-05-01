@echo off
setlocal

set "CONFIG="

if not "%~1" == "" (
    if /I not "%~1" == "/Debug" (
        if /I not "%~1" == "/Release" (
            msg "%username%" Invalid argument: choose /Debug or /Release, no argument sets /Release as default
            exit /b 1
        )
    )
)

if "%~1" == "/Release" (
    set "CONFIG=/Release"
)

if "%~1" == "" (
    set "CONFIG=/Release"
)

if "%~1" == "/Debug" (
    set "CONFIG=/Debug"
)


call "%~dp0update_resources.bat" "%CONFIG%"

call "%~dp0copy_dlls.bat" "%CONFIG%"
