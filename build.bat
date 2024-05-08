@echo off
where cl.exe > nul 2>&1
if %errorlevel% neq 0 (call setup.bat)

ctime -begin compile_time.txt

rem DEBUG
cl src/main.c /Iinclude /Yccgltf.h /Od /MTd /Zi /Fegltf2custom.exe /nologo /GR /EHa /Oi /fp:fast /Fm /FC /subsystem:windows

rem RELEASE
rem cl src/main.c /Iinclude /Yccgltf.h /Ox /MT /DNDEBUG /Fegltf2custom.exe /nologo /GR /EHa /Oi /fp:fast /Fm /FC

ctime -end compile_time.txt
del compile_time.txt > NUL
