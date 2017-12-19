@ set CDIR=%CD:\=//%
@ set CDIR1=%CD%
@ set _NIOS2EDS_ROOT=%SOPC_KIT_NIOS2:\=//%
cd %SOPC_KIT_NIOS2%
%QUARTUS_ROOTDIR%\bin64\cygwin\bin\bash.exe -c "%_NIOS2EDS_ROOT%/nios2_command_shell.sh system-console.exe -cli --script=%CDIR%//capture.tcl"

