@echo off
where cl.exe > nul 2>&1
if %errorlevel% neq 0 (call setup.bat)

rem DEBUG
cl src/main.c /Fegltf2custom.exe /Zi /nologo /Iinclude

rem RELEASE
rem cl src/main.c /DNDEBUG /O2 /Fegltf2custom.exe /nologo /Iinclude
