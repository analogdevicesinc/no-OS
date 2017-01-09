@echo off

set XMD_DIR=C:\Xilinx\SDK\2015.4\bin

if "%~1"=="" goto user
set hdf_destination="%~1"
set build_destination="%~2"
if "%~2"=="" set build_destination="./"

goto end

:user
echo Enter the system_top.hdf location
echo "example => C:/workspace/my_proj1_hdf/system_top.hdf"
set /p hdf_destination=

echo Enter the project build folder
echo "example => C:/my_workspaces Note: use "/" instead of "\""
set /p build_destination=
:end

%XMD_DIR%\xsct ..\build_scripts\xilinx\create_sdk_project.tcl %hdf_destination% %build_destination%

echo Done