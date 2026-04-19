@echo off
setlocal

set "ROOT=%~dp0.."
set "DST=%ROOT%\x64\Debug"

robocopy "%ROOT%\ThirdParty\assimp" "%DST%" "assimp-vc145-mt.dll"
robocopy "%ROOT%\ThirdParty\glfw" "%DST%" "glfw3.dll"

exit /b 0