@echo off
setlocal

set "ROOT=%~dp0.."
set "SRC=%ROOT%\Resources"
set "DST=%ROOT%\x64\Debug\Resources"

robocopy "%SRC%" "%DST%" /E /XO /XX

endlocal