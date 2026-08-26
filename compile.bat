@echo off
title Bee-Gold-Collector Crash Guard
color 0A

echo ========================================================
echo Searching for Visual Studio...
echo ========================================================

set "VS_PATH="
for /f "delims=" %%i in ('where /r "C:\Program Files\Microsoft Visual Studio" vcvarsall.bat 2^>nul') do (
    set "VS_PATH=%%i"
    goto FOUND_VS
)

:FOUND_VS
if "%VS_PATH%"=="" (
    echo.
    echo [ERROR] Visual Studio could not be found!
    goto KEEP_ALIVE
)

echo Found file: %VS_PATH%
echo Loading environment...
call "%VS_PATH%" x64 >nul

echo.
echo Starting compilation...
echo --------------------------------------------------------

:: Compilation Command
cl /EHsc /std:c++17 main.cpp /I"C:\Bee-Gold-Collector-Windows-11-Edition\compilelibs\include" /link /LIBPATH:"C:\Bee-Gold-Collector-Windows-11-Edition\compilelibs\libs\SFML" sfml-graphics.lib sfml-window.lib sfml-system.lib opengl32.lib winmm.lib gdi32.lib

if %errorlevel% neq 0 (
    echo.
    echo --------------------------------------------------------
    echo [FAILED] An error occurred during compilation! (Check the error codes above)
    echo --------------------------------------------------------
) else (
    echo.
    echo --------------------------------------------------------
    echo [SUCCESS] No errors! Compilation completed.
    echo Output location: %CD%\main.exe
    echo --------------------------------------------------------
)

:KEEP_ALIVE
echo.
echo ========================================================
echo KEEPING WINDOW OPEN. Press X or ENTER to close.
echo ========================================================
pause >nul