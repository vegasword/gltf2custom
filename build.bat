@echo off

set DEBUG=1
set IFLAGS=/Iinclude /Yccgltf.h
set CFLAGS=/Fegltf2custom /nologo /GR /EHa /Oi /fp:fast /FC /link /INCREMENTAL:NO /opt:ref

rem INSTALL THE LATEST VERSION OF BUILD TOOLS AND ITS SETUP BATCH SCRIPT
rem @ https://github.com/Data-Oriented-House/PortableBuildTools/releases
where cl.exe > nul 2>&1
if %errorlevel% neq 0 (call setup.bat)

ctime -begin compile_time.txt

if %DEBUG% == "1" (
  cl src/main.c %IFLAGS% /Od /MTd /Zi /Fm %CFLAGS%
) else (
  cl src/main.c %IFLAGS% /Ox /MT /DNDEBUG %CFLAGS%
)

ctime -end compile_time.txt
del compile_time.txt > NUL
