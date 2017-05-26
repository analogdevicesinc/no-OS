@echo off

IF "%PROCESSOR_ARCHITECTURE%"=="x86" set XMD_DIR=C:\Xilinx\SDK\2016.2\bin
IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" set XMD_DIR=C:\Xilinx\SDK\2016.2\bin

%XMD_DIR%\xmd.bat -tcl capture.tcl

pause
