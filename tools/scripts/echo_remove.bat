:: Search and replace 'echo' string in input text file with single space
@echo off 
setlocal enabledelayedexpansion
setlocal enableextensions

set "search=echo"
set "replace="

:: Loop through all input text files
for %%x in (%*) do (
    echo %%x
    for /f "delims=" %%i in ('type "%%x" ^& break ^> "%%x" ') do (
        set "line=%%i"
        >>"%%x" echo(!line:%search%=%replace%!)
    )
)
