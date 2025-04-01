@echo off
:: This script searches for VS shell and runs CMake configure and build.
:: Allows for setuping the CMake project regardless of VS version and such.

:: Setup defaults (use default if the user doesn't provide one)
set BUILD_DIR=build
set MODE=Debug

:: Find the latest installed Visual Studio
call "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath > vs_path.txt
set /p VS_PATH=<vs_path.txt
del vs_path.txt

:: Echo VS_PATH for debugging
echo Found Visual Studio at: %VS_PATH%

:: Check if VS_PATH is valid
if not exist "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" (
    echo Could not find Visual Studio! Exiting...
    exit /b 1
)

:: Run the Visual Studio command prompt (detects architecture automatically)
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

:: Check if CMakeCache.txt exists and delete it
if exist "%BUILD_DIR%\CMakeCache.txt" (
    echo Deleting existing CMake cache...
    del /f /q "%BUILD_DIR%\CMakeCache.txt"
)

:: Run CMake to generate build files
echo Generating CMake build files...
cmake -B %BUILD_DIR% -G "Visual Studio 17" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

:: Build the project
echo Building CMake project...
cmake --build %BUILD_DIR% --config %MODE%
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Success!

:: Countdown before exiting
for /l %%i in (10,-1,1) do (
	echo Exiting in %%i seconds...
	timeout /nobreak /t 1 >nul
)
exit /b 0