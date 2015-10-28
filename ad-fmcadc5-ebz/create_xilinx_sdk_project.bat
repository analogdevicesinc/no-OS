@echo off

set XMD_DIR=C:\Xilinx\SDK\2015.2\bin

if "%~2"=="" goto user
set build_destination="%~1"
set hdf_destination="%~2"

goto end

:user
echo Enter the project build folder
echo "example => C:/my_workspaces Note: use "/" instead of "\""
set /p build_destination=

echo Enter the system_top.hdf location
echo "example => C:/workspace/my_proj1_hdf/system_top.hdf"
set /p hdf_destination=
:end

%XMD_DIR%\xsct -s ..\build_scripts\xilinx\create_sdk_project.tcl %build_destination% %hdf_destination%

echo Done