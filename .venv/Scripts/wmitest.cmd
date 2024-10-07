@echo off
for /l %%n in (24,1,31) do if exist c:\python%%n\python.exe (echo. & echo python%%n & c:\python%%n\python.exe -W ignore wmitest.py)
pause
