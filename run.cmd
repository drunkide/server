@echo off
cd /D "%~dp0" || exit /B 1
tools\pour_wrapper_windows.exe --build win32:msvc_2022 || exit /B 1
call go.cmd run main.go || exit /B 1
