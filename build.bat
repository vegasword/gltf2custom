@echo off
SET IFLAGS=/Iinclude /Yccgltf.h
SET CFLAGS=/Fegltf2custom /nologo /GR /EHa /Oi /fp:fast /Fm /FC /link ^
/INCREMENTAL:NO /opt:ref

rem INSTALL THE LATEST VERSION OF BUILD TOOLS AND ITS SETUP BATCH SCRIPT
rem @ https://github.com/Data-Oriented-House/PortableBuildTools/releases
where cl.exe > nul 2>&1
if %errorlevel% neq 0 (call setup.bat)

ctime -begin compile_time.txt

rem DEBUG
cl src/main.c %IFLAGS% /Od /MTd /Zi %CFLAGS%

rem RELEASE
rem cl src/main.c %IFLAGS% /Ox /MT /DNDEBUG %CFLAGS%

ctime -end compile_time.txt
del compile_time.txt > NUL
