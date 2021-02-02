@echo off

IF "%PROCESSOR_ARCHITECTURE%"=="x86" set XMD_DIR=C:\Xilinx\14.6\ISE_DS\EDK\bin\nt
IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" set XMD_DIR=C:\Xilinx\14.6\ISE_DS\EDK\bin\nt64

REM Note: The data_capture.tcl script must to run with one argument, which has
REM to be the name of the directory, were the project is located.
REM Usually the project directory name has the following structure:
REM                 cf_<device_name>_<development_board>

%XMD_DIR%\xmd.exe -tcl data_capture.tcl cf_ad7091_

pause
