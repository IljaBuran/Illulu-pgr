@echo off
setlocal

call "%~dp0update_resources.bat"

call "%~dp0copy_dlls.bat"

exit /b 0