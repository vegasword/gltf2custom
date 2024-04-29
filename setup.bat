@echo off

set WindowsSDKDir=C:\BuildTools\Windows Kits\10
set WindowsSDKVersion=10.0.22621.0
set VCToolsInstallDir=C:\BuildTools\VC\Tools\MSVC\14.38.33130\
set VSCMD_ARG_TGT_ARCH=x64

set SDK_BIN=C:\BuildTools\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64;C:\BuildTools\Windows Kits\10\bin\10.0.22621.0\x64;C:\BuildTools\Windows Kits\10\bin\10.0.22621.0\x64\ucrt
set PATH=%SDK_BIN%;%PATH%
set INCLUDE=C:\BuildTools\VC\Tools\MSVC\14.38.33130\include;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\ucrt;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\shared;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\um;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\winrt;C:\BuildTools\Windows Kits\10\Include\10.0.22621.0\cppwinrt
set LIB=C:\BuildTools\VC\Tools\MSVC\14.38.33130\lib\x64;C:\BuildTools\Windows Kits\10\Lib\10.0.22621.0\ucrt\x64;C:\BuildTools\Windows Kits\10\Lib\10.0.22621.0\um\x64
