@ECHO off
GOTO start
:find_dp0
SET dp0=%~dp0
EXIT /b
:start
SETLOCAL
CALL :find_dp0
"C:\Users\NBroderi\AppData\Roaming\xPacks\@xpack-dev-tools\openocd\0.12.0-4.1\.content\bin\openocd.exe"   %*
