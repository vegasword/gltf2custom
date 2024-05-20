@echo off

set WindowsSDKDir=C:\BuildTools\Windows Kits\10
set WindowsSDKVersion=10.0.22621.0
set VCToolsInstallDir=C:\BuildTools\VC\Tools\MSVC\14.39.33519\
set VSCMD_ARG_TGT_ARCH=x64

set MSVC_BIN=C:\BuildTools\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64
set SDK_BIN=C:\BuildTools\Windows Kits\10\bin\10.0.22621.0\x64;C:\BuildTools\Windows Kits\10\bin\10.0.22621.0\x64\ucrt
set PATH=%MSVC_BIN%;%SDK_BIN%;%PATH%
set INCLUDE=C:\BuildTools\VC\Tools\MSVC\14.39.33519\include;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\ucrt;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\shared;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\um;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\winrt;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\cppwinrt
set LIB=C:\BuildTools\VC\Tools\MSVC\14.39.33519\lib\x64;C:\BuildTools\Windows Kits\10\Lib\10.0.22621.0\ucrt\x64;C:\BuildTools\Windows Kits\10\Lib\10.0.22621.0\um\x64
